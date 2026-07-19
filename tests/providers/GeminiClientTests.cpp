#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QtTest>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/GeminiClient.hpp"
#include "infrastructure/providers/ProviderRequest.hpp"
#include "FakeNetworkExecutor.hpp"

namespace domain = icodental::domain;
namespace providers = icodental::infrastructure::providers;

class GeminiClientTests : public QObject {
    Q_OBJECT
    private slots:
        void analyze_returnsSuccess_onValidResponse();
        void analyze_returnsFailure_onNetworkError();
        void analyze_returnsFailure_onInvalidProvider();
        void analyze_returnsFailure_onEmptyApiKey();
        void analyze_returnsFailure_onMissingImageFile();
        void analyze_postsExpectedEndpointAndPayload();
};

void GeminiClientTests::analyze_returnsSuccess_onValidResponse() {
    providers::FakeNetworkExecutor fakeExecutor;
    fakeExecutor.nextResult = {
        true,
        R"json(
            {
              "candidates": [
                {
                  "content": {
                    "parts": [
                      { "text": "Lab summary from Gemini." }
                    ]
                  }
                }
              ]
            }
        )json",
        QString()
    };

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString imagePath = tempDir.filePath("sample.jpg");
    QFile imageFile(imagePath);
    QVERIFY(imageFile.open(QIODevice::WriteOnly));
    imageFile.write("fake-image-content");
    imageFile.close();

    providers::GeminiClient client("fake-api-key", &fakeExecutor);

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this case.",
        imagePath);

    const auto response = client.analyze(request);

    QVERIFY(response.success());
    QVERIFY(response.isValid());
    QCOMPARE(response.summaryText(), QString("Lab summary from Gemini."));
    QVERIFY(response.errorMessage().isEmpty());
    QCOMPARE(fakeExecutor.callCount, 1);
}

void GeminiClientTests::analyze_returnsFailure_onNetworkError() {
    providers::FakeNetworkExecutor fakeExecutor;
    fakeExecutor.nextResult = {
        false,
        QByteArray("upstream failure body"),
        QString("Simulated network error")
    };

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString imagePath = tempDir.filePath("sample.jpg");
    QFile imageFile(imagePath);
    QVERIFY(imageFile.open(QIODevice::WriteOnly));
    imageFile.write("fake-image-content");
    imageFile.close();

    providers::GeminiClient client("fake-api-key", &fakeExecutor);

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this case.",
        imagePath);

    const auto response = client.analyze(request);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Simulated network error"));
    QCOMPARE(response.rawResponse(), QString("upstream failure body"));
    QCOMPARE(fakeExecutor.callCount, 1);
}

void GeminiClientTests::analyze_returnsFailure_onInvalidProvider() {
    providers::FakeNetworkExecutor fakeExecutor;
    providers::GeminiClient client("fake-api-key", &fakeExecutor);

    const providers::ProviderRequest request(
        static_cast<domain::ProviderType>(999),
        "gemini-1.5-pro",
        "Summarize this case.",
        "/tmp/does-not-matter.jpg");

    const auto response = client.analyze(request);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("GeminiClient received a non-Gemini request."));
    QCOMPARE(fakeExecutor.callCount, 0);
}

void GeminiClientTests::analyze_returnsFailure_onEmptyApiKey() {
    providers::FakeNetworkExecutor fakeExecutor;
    providers::GeminiClient client("", &fakeExecutor);

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this case.",
        "/tmp/does-not-matter.jpg");

    const auto response = client.analyze(request);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini API key is empty."));
    QCOMPARE(fakeExecutor.callCount, 0);
}

void GeminiClientTests::analyze_returnsFailure_onMissingImageFile() {
    providers::FakeNetworkExecutor fakeExecutor;
    providers::GeminiClient client("fake-api-key", &fakeExecutor);

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-pro",
        "Summarize this case.",
        "/path/that/does/not/exist.jpg");

    const auto response = client.analyze(request);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QVERIFY(response.errorMessage().startsWith("Failed to open image file: "));
    QCOMPARE(fakeExecutor.callCount, 0);
}

void GeminiClientTests::analyze_postsExpectedEndpointAndPayload() {
    providers::FakeNetworkExecutor fakeExecutor;
    fakeExecutor.nextResult = {
        true,
        R"json(
            {
              "candidates": [
                {
                  "content": {
                    "parts": [
                      { "text": "OK" }
                    ]
                  }
                }
              ]
            }
        )json",
        QString()
    };

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString imagePath = tempDir.filePath("sample.jpg");
    QFile imageFile(imagePath);
    QVERIFY(imageFile.open(QIODevice::WriteOnly));
    const QByteArray imageBytes("binary-image-data");
    imageFile.write(imageBytes);
    imageFile.close();

    providers::GeminiClient client("fake-api-key", &fakeExecutor);

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this case.",
        imagePath);

    const auto response = client.analyze(request);

    QVERIFY(response.success());
    QCOMPARE(fakeExecutor.callCount, 1);

    QCOMPARE(
        fakeExecutor.lastUrl.toString(),
        QString("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=fake-api-key"));

    QVERIFY(fakeExecutor.lastPayload.contains("contents"));
    QVERIFY(fakeExecutor.lastPayload.value("contents").isArray());

    const QJsonArray contents = fakeExecutor.lastPayload.value("contents").toArray();
    QCOMPARE(contents.size(), 1);

    const QJsonObject firstContent = contents.at(0).toObject();
    QVERIFY(firstContent.contains("parts"));
    QVERIFY(firstContent.value("parts").isArray());

    const QJsonArray parts = firstContent.value("parts").toArray();
    QCOMPARE(parts.size(), 2);

    const QJsonObject textPart = parts.at(0).toObject();
    QCOMPARE(textPart.value("text").toString(), QString("Summarize this case."));

    const QJsonObject inlinePart = parts.at(1).toObject();
    const QJsonObject inlineData = inlinePart.value("inlineData").toObject();

    QCOMPARE(inlineData.value("mimeType").toString(), QString("image/jpeg"));
    QCOMPARE(
        inlineData.value("data").toString(),
        QString::fromLatin1(imageBytes.toBase64()));
}

QTEST_APPLESS_MAIN(GeminiClientTests)

#include "GeminiClientTests.moc"