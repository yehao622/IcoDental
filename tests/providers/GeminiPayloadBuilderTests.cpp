#include <QJsonArray>
#include <QJsonObject>
#include <QtTest>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/GeminiPayloadBuilder.hpp"
#include "infrastructure/providers/ProviderRequest.hpp"

namespace domain = icodental::domain;
namespace providers = icodental::infrastructure::providers;

class GeminiPayloadBuilderTests : public QObject {
    Q_OBJECT

    private slots:
        void build_includesPromptAndInlineImageData();
};

void GeminiPayloadBuilderTests::build_includesPromptAndInlineImageData() {
    providers::GeminiPayloadBuilder builder;

    const providers::ProviderRequest request(
        domain::ProviderType::Gemini,
        "gemini-2.5-flash",
        "Summarize this lab case.",
        "/tmp/sample.jpg");

    const QByteArray imageBytes("fake-image-binary");
    const QJsonObject payload = builder.build(request, imageBytes);

    QVERIFY(payload.contains("contents"));
    QVERIFY(payload.value("contents").isArray());

    const QJsonArray contents = payload.value("contents").toArray();
    QCOMPARE(contents.size(), 1);

    const QJsonObject firstContent = contents.at(0).toObject();
    QVERIFY(firstContent.contains("parts"));
    QVERIFY(firstContent.value("parts").isArray());

    const QJsonArray parts = firstContent.value("parts").toArray();
    QCOMPARE(parts.size(), 2);

    const QJsonObject textPart = parts.at(0).toObject();
    QCOMPARE(textPart.value("text").toString(), QString("Summarize this lab case."));

    const QJsonObject inlinePart = parts.at(1).toObject();
    QVERIFY(inlinePart.contains("inlineData"));

    const QJsonObject inlineData = inlinePart.value("inlineData").toObject();
    QCOMPARE(inlineData.value("mimeType").toString(), QString("image/jpeg"));
    QCOMPARE(inlineData.value("data").toString(), QString::fromLatin1(imageBytes.toBase64()));
}

QTEST_APPLESS_MAIN(GeminiPayloadBuilderTests)

#include "GeminiPayloadBuilderTests.moc"