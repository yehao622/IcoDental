#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QTemporaryDir>
#include <QtTest>

#include "application/AnalysisOrchestrator.hpp"
#include "application/AnalysisRequest.hpp"
#include "domain/ImageFingerprint.hpp"
#include "domain/ProviderType.hpp"
#include "infrastructure/cache/AnalysisCacheEntry.hpp"
#include "infrastructure/cache/AnalysisCacheRepository.hpp"
#include "infrastructure/cache/CacheDatabase.hpp"
#include "infrastructure/filesystem/FileScanner.hpp"
#include "infrastructure/filesystem/ImageHasher.hpp"

namespace app = icodental::application;
namespace cache = icodental::infrastructure::cache;
namespace fs = icodental::infrastructure::filesystem;
namespace domain = icodental::domain;

class AnalysisOrchestratorTests : public QObject {
    Q_OBJECT

    private slots:
        void buildPlan_returnsErrorForInvalidRequest();
        void buildPlan_explicitFilesWithCacheMiss_returnsPendingItem();
        void buildPlan_explicitFilesWithCacheHit_returnsCachedItem();
        void buildPlan_forceRefresh_bypassesCache();

    private:
        static QString writeFile(QTemporaryDir& tempDir, const QString& fileName, const QByteArray& content);
        static void cleanupConnection(const QString& connectionName);
};

QString AnalysisOrchestratorTests::writeFile(
    QTemporaryDir& tempDir,
    const QString& fileName,
    const QByteArray& content) 
{
    const QString path = tempDir.filePath(fileName);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return {};
    }

    file.write(content);
    file.close();
    return path;
}

void AnalysisOrchestratorTests::cleanupConnection(const QString& connectionName) {
    if (connectionName.isEmpty()) {
        return;
    }

    {
        QSqlDatabase db = QSqlDatabase::database(connectionName, false);
        if (db.isValid() && db.isOpen()) {
            db.close();
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
}

void AnalysisOrchestratorTests::buildPlan_returnsErrorForInvalidRequest() {
    fs::FileScanner fileScanner;
    fs::ImageHasher imageHasher;

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString dbPath = tempDir.filePath("invalid_request.sqlite");

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);
        app::AnalysisOrchestrator orchestrator(fileScanner, imageHasher, repository);

        const app::AnalysisRequest request(
            app::AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {},
            domain::ProviderType::Unknown,
            QString(),
            false);

        const auto plan = orchestrator.buildPlan(request);

        QVERIFY(plan.hasErrors());
        QCOMPARE(plan.errors().size(), 1);
        QVERIFY(plan.cachedItems().isEmpty());
        QVERIFY(plan.pendingItems().isEmpty());
    }

    cleanupConnection(connectionName);
}

void AnalysisOrchestratorTests::buildPlan_explicitFilesWithCacheMiss_returnsPendingItem() {
    fs::FileScanner fileScanner;
    fs::ImageHasher imageHasher;

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = writeFile(tempDir, "sample.png", "fake-image-content");
    QVERIFY(!filePath.isEmpty());

    const QString dbPath = tempDir.filePath("cache_miss.sqlite");

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);
        app::AnalysisOrchestrator orchestrator(fileScanner, imageHasher, repository);

        const app::AnalysisRequest request(
            app::AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {filePath},
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            false);

        const auto plan = orchestrator.buildPlan(request);

        QVERIFY(!plan.hasErrors());
        QVERIFY(plan.cachedItems().isEmpty());
        QCOMPARE(plan.pendingItems().size(), 1);
        QCOMPARE(plan.pendingItems().first().filePath(), filePath);
        QVERIFY(plan.pendingItems().first().fingerprint().isValid());
        QVERIFY(!plan.pendingItems().first().hasCachedEntry());
    }

    cleanupConnection(connectionName);
}

void AnalysisOrchestratorTests::buildPlan_explicitFilesWithCacheHit_returnsCachedItem() {
    fs::FileScanner fileScanner;
    fs::ImageHasher imageHasher;

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = writeFile(tempDir, "cached.jpg", "fake-image-content");
    QVERIFY(!filePath.isEmpty());

    const QString dbPath = tempDir.filePath("cache_hit.sqlite");
    const QDateTime now = QDateTime::currentDateTimeUtc();

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);

        const auto fingerprint = imageHasher.hashFile(filePath);
        QVERIFY(fingerprint.isValid());

        const cache::AnalysisCacheEntry entry(
            fingerprint,
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            "cached summary",
            now,
            now);

        QVERIFY(entry.isValid());
        QVERIFY(repository.save(entry));

        app::AnalysisOrchestrator orchestrator(fileScanner, imageHasher, repository);

        const app::AnalysisRequest request(
            app::AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {filePath},
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            false);

        const auto plan = orchestrator.buildPlan(request);

        QVERIFY(!plan.hasErrors());
        QCOMPARE(plan.cachedItems().size(), 1);
        QVERIFY(plan.pendingItems().isEmpty());
        QCOMPARE(plan.cachedItems().first().filePath(), filePath);
        QVERIFY(plan.cachedItems().first().hasCachedEntry());
        QVERIFY(plan.cachedItems().first().cachedEntry().has_value());
        QCOMPARE(plan.cachedItems().first().cachedEntry()->model(), QString("gemini-1.5-pro"));
    }

    cleanupConnection(connectionName);
}

void AnalysisOrchestratorTests::buildPlan_forceRefresh_bypassesCache() {
    fs::FileScanner fileScanner;
    fs::ImageHasher imageHasher;

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = writeFile(tempDir, "refresh.jpeg", "fake-image-content");
    QVERIFY(!filePath.isEmpty());

    const QString dbPath = tempDir.filePath("force_refresh.sqlite");
    const QDateTime now = QDateTime::currentDateTimeUtc();

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);

        const auto fingerprint = imageHasher.hashFile(filePath);
        QVERIFY(fingerprint.isValid());

        const cache::AnalysisCacheEntry entry(
            fingerprint,
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            "cached summary",
            now,
            now);

        QVERIFY(repository.save(entry));

        app::AnalysisOrchestrator orchestrator(fileScanner, imageHasher, repository);

        const app::AnalysisRequest request(
            app::AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {filePath},
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            true);

        const auto plan = orchestrator.buildPlan(request);

        QVERIFY(!plan.hasErrors());
        QVERIFY(plan.cachedItems().isEmpty());
        QCOMPARE(plan.pendingItems().size(), 1);
        QCOMPARE(plan.pendingItems().first().filePath(), filePath);
        QVERIFY(!plan.pendingItems().first().hasCachedEntry());
    }

    cleanupConnection(connectionName);
}

QTEST_APPLESS_MAIN(AnalysisOrchestratorTests)

#include "AnalysisOrchestratorTests.moc"