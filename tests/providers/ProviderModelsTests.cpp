#include <QtTest>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/ProviderRequest.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"

namespace domain = icodental::domain;
namespace providers = icodental::infrastructure::providers;

class ProviderModelsTests : public QObject {
    Q_OBJECT

    private slots:
        void providerRequest_isValid_whenAllFieldsPresent();
        void providerRequest_isInvalid_whenProviderUnknown();
        void providerRequest_isInvalid_whenModelEmpty();
        void providerRequest_isInvalid_whenPromptEmpty();
        void providerRequest_isInvalid_whenImagePathEmpty();

        void providerResponse_isValid_whenSuccessfulAndSummaryPresent();
        void providerResponse_isInvalid_whenFailed();
        void providerResponse_isInvalid_whenSummaryEmpty();
        void providerResponse_getters_returnStoredValues();
};

void ProviderModelsTests::providerRequest_isValid_whenAllFieldsPresent() {
    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this dental lab image.",
        "/tmp/sample.jpg");

    QVERIFY(request.isValid());
    QCOMPARE(static_cast<int>(request.provider()), static_cast<int>(domain::ProviderType::Gemini));
    QCOMPARE(request.model(), QString("gemini-2.5-flash"));
    QCOMPARE(request.prompt(), QString("Summarize this dental lab image."));
    QCOMPARE(request.imagePath(), QString("/tmp/sample.jpg"));
}

void ProviderModelsTests::providerRequest_isInvalid_whenProviderUnknown() {
    const providers::ProviderRequest request(
        domain::ProviderType::Unknown,
        "gemini-2.5-flash",
        "Summarize this dental lab image.",
        "/tmp/sample.jpg");

    QVERIFY(!request.isValid());
}

void ProviderModelsTests::providerRequest_isInvalid_whenModelEmpty() {
    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "",
        "Summarize this dental lab image.",
        "/tmp/sample.jpg");

    QVERIFY(!request.isValid());
}

void ProviderModelsTests::providerRequest_isInvalid_whenPromptEmpty() {
    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "",
        "/tmp/sample.jpg");

    QVERIFY(!request.isValid());
}

void ProviderModelsTests::providerRequest_isInvalid_whenImagePathEmpty() {
    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this dental lab image.",
        "");

    QVERIFY(!request.isValid());
}

void ProviderModelsTests::providerResponse_isValid_whenSuccessfulAndSummaryPresent() {
    const providers::ProviderResponse response(
        true,
        "Tooth #4 crown remake requested.",
        R"({"status":"ok"})",
        "");

    QVERIFY(response.success());
    QVERIFY(response.isValid());
    QCOMPARE(response.summaryText(), QString("Tooth #4 crown remake requested."));
    QCOMPARE(response.rawResponse(), QString(R"({"status":"ok"})"));
    QCOMPARE(response.errorMessage(), QString(""));
}

void ProviderModelsTests::providerResponse_isInvalid_whenFailed() {
    const providers::ProviderResponse response(
        false,
        "",
        R"({"status":"error"})",
        "Request failed.");

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Request failed."));
}

void ProviderModelsTests::providerResponse_isInvalid_whenSummaryEmpty() {
    const providers::ProviderResponse response(
        true,
        "",
        R"({"status":"ok"})",
        "");

    QVERIFY(response.success());
    QVERIFY(!response.isValid());
}

void ProviderModelsTests::providerResponse_getters_returnStoredValues() {
    const providers::ProviderResponse response(
        false,
        "partial summary",
        R"({"raw":"payload"})",
        "Timeout");

    QCOMPARE(response.success(), false);
    QCOMPARE(response.summaryText(), QString("partial summary"));
    QCOMPARE(response.rawResponse(), QString(R"({"raw":"payload"})"));
    QCOMPARE(response.errorMessage(), QString("Timeout"));
}

QTEST_APPLESS_MAIN(ProviderModelsTests)

#include "ProviderModelsTests.moc"