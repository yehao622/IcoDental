#include <QSignalSpy>
#include <QtConcurrent>
#include <QtTest>

#include "domain/CaseAnalysisResult.hpp"
#include "domain/ImageFingerprint.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"
#include "ui/viewmodels/BatchAnalysisController.hpp"

namespace {
    using icodental::domain::CaseAnalysisResult;
    using icodental::domain::ImageFingerprint;
    using icodental::infrastructure::providers::ProviderResponse;
    using icodental::ui::BatchAnalysisController;
    using icodental::ui::BatchAnalysisItem;
    using icodental::ui::BatchItemState;

    CaseAnalysisResult makeResult(const QString& name) {
        return CaseAnalysisResult(
            "Dr. Kim",
            "Onion Dental",
            name,
            "Crown",
            "#4",
            "A2",
            "Refabricate crown.",
            "High confidence.",
            "Refabricate crown.");
    }

    ProviderResponse successfulResponse(const QString& name) {
        const CaseAnalysisResult result = makeResult(name);

        return ProviderResponse(
            true,
            result.rawProviderText(),
            "{\"provider\":\"test\"}",
            QString(),
            result);
    }

    ProviderResponse failedResponse(const QString& message) {
        return ProviderResponse(
            false,
            QString(),
            "{\"provider\":\"test\"}",
            message);
    }

    BatchAnalysisItem pendingItem(
        const QString& path,
        const QString& fingerprint) {
        return BatchAnalysisItem{
            path,
            ImageFingerprint(fingerprint),
            BatchItemState::Pending,
            QString(),
            std::nullopt
        };
    }

    BatchAnalysisItem cachedItem(
        const QString& path,
        const QString& fingerprint) {
        return BatchAnalysisItem{
            path,
            ImageFingerprint(fingerprint),
            BatchItemState::Cached,
            "Loaded from cache.",
            makeResult("Cached patient")
        };
    }
}

class BatchAnalysisControllerTests : public QObject {
    Q_OBJECT
    private slots:
        void start_rejectsEmptyItems();
        void start_marksFirstPendingItemRunning();
        void processesPendingItemsSequentially();
        void continuesAfterProviderFailure();
        void cancellationPreventsNextItemFromStarting();
        void cachedItemsCountAsCompleted();
};

void BatchAnalysisControllerTests::start_rejectsEmptyItems() {
    BatchAnalysisController controller;

    QSignalSpy failedSpy(
        &controller,
        &BatchAnalysisController::batchFailedToStart);

    controller.start(
        {},
        [](const QString&, const ImageFingerprint&) {
            return QtConcurrent::run([] {
                return successfulResponse("Unused");
            });
        });

    QCOMPARE(failedSpy.count(), 1);
    QVERIFY(!controller.isRunning());
    QCOMPARE(controller.totalCount(), 0);
}

void BatchAnalysisControllerTests::start_marksFirstPendingItemRunning() {
    BatchAnalysisController controller;

    QSignalSpy itemUpdatedSpy(
        &controller,
        &BatchAnalysisController::itemUpdated);

    controller.start(
        {
            pendingItem("first.jpg", QString(64, 'a')),
            pendingItem("second.jpg", QString(64, 'b'))
        },
        [](const QString&, const ImageFingerprint&) {
            return QtConcurrent::run([] {
                QThread::msleep(100);
                return successfulResponse("Patient");
            });
        });

    QCOMPARE(controller.items().size(), 2);
    QCOMPARE(
        controller.items().at(0).state,
        BatchItemState::Running);
    QCOMPARE(
        controller.items().at(1).state,
        BatchItemState::Pending);
    QVERIFY(itemUpdatedSpy.count() >= 1);
}

void BatchAnalysisControllerTests::processesPendingItemsSequentially() {
    BatchAnalysisController controller;

    QStringList processedPaths;

    QSignalSpy finishedSpy(
        &controller,
        &BatchAnalysisController::batchFinished);

    controller.start(
        {
            pendingItem("first.jpg", QString(64, 'a')),
            pendingItem("second.jpg", QString(64, 'b'))
        },
        [&processedPaths](
            const QString& imagePath,
            const ImageFingerprint&) {
            processedPaths.append(imagePath);

            return QtConcurrent::run(
                [imagePath] {
                    return successfulResponse(imagePath);
                });
        });

    QTRY_COMPARE(finishedSpy.count(), 1);

    QCOMPARE(
        processedPaths,
        QStringList({"first.jpg", "second.jpg"}));

    QCOMPARE(
        controller.items().at(0).state,
        BatchItemState::Succeeded);
    QCOMPARE(
        controller.items().at(1).state,
        BatchItemState::Succeeded);

    QCOMPARE(controller.completedCount(), 2);
    QVERIFY(!controller.isRunning());
}

void BatchAnalysisControllerTests::continuesAfterProviderFailure() {
    BatchAnalysisController controller;

    QStringList processedPaths;

    QSignalSpy finishedSpy(
        &controller,
        &BatchAnalysisController::batchFinished);

    controller.start(
        {
            pendingItem("first.jpg", QString(64, 'a')),
            pendingItem("second.jpg", QString(64, 'b'))
        },
        [&processedPaths](
            const QString& imagePath,
            const ImageFingerprint&) {
            processedPaths.append(imagePath);

            return QtConcurrent::run(
                [imagePath] {
                    if (imagePath == "first.jpg") {
                        return failedResponse("Test provider failure.");
                    }

                    return successfulResponse("Second patient");
                });
        });

    QTRY_COMPARE(finishedSpy.count(), 1);

    QCOMPARE(
        processedPaths,
        QStringList({"first.jpg", "second.jpg"}));

    QCOMPARE(
        controller.items().at(0).state,
        BatchItemState::Failed);
    QCOMPARE(
        controller.items().at(1).state,
        BatchItemState::Succeeded);

    QCOMPARE(controller.items().at(0).message,
             QString("Test provider failure."));
}

void BatchAnalysisControllerTests::cancellationPreventsNextItemFromStarting() {
    BatchAnalysisController controller;

    QStringList processedPaths;

    QSignalSpy finishedSpy(
        &controller,
        &BatchAnalysisController::batchFinished);

    controller.start(
        {
            pendingItem("first.jpg", QString(64, 'a')),
            pendingItem("second.jpg", QString(64, 'b'))
        },
        [&processedPaths](
            const QString& imagePath,
            const ImageFingerprint&) {
            processedPaths.append(imagePath);

            return QtConcurrent::run(
                [] {
                    QThread::msleep(50);
                    return successfulResponse("First patient");
                });
        });

    controller.cancel();

    QTRY_COMPARE(finishedSpy.count(), 1);

    QCOMPARE(processedPaths, QStringList({"first.jpg"}));

    QCOMPARE(
        controller.items().at(0).state,
        BatchItemState::Succeeded);
    QCOMPARE(
        controller.items().at(1).state,
        BatchItemState::Cancelled);

    QCOMPARE(controller.completedCount(), 2);
    QVERIFY(!controller.isRunning());
}

void BatchAnalysisControllerTests::cachedItemsCountAsCompleted() {
    BatchAnalysisController controller;

    QSignalSpy finishedSpy(
        &controller,
        &BatchAnalysisController::batchFinished);

    controller.start(
        {
            cachedItem("cached.jpg", QString(64, 'a')),
            pendingItem("pending.jpg", QString(64, 'b'))
        },
        [](const QString&, const ImageFingerprint&) {
            return QtConcurrent::run([] {
                return successfulResponse("Pending patient");
            });
        });

    QTRY_COMPARE(finishedSpy.count(), 1);

    QCOMPARE(
        controller.items().at(0).state,
        BatchItemState::Cached);
    QCOMPARE(
        controller.items().at(1).state,
        BatchItemState::Succeeded);

    QCOMPARE(controller.completedCount(), 2);

    const QList<QVariant> finishedArguments =
        finishedSpy.takeFirst();

    QCOMPARE(finishedArguments.at(0).toInt(), 2);
    QCOMPARE(finishedArguments.at(1).toInt(), 0);
    QCOMPARE(finishedArguments.at(2).toInt(), 0);
}

QTEST_MAIN(BatchAnalysisControllerTests)

#include "BatchAnalysisControllerTests.moc"