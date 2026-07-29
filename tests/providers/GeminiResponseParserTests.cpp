#include <QtTest>

#include "infrastructure/providers/GeminiResponseParser.hpp"

namespace providers = icodental::infrastructure::providers;

class GeminiResponseParserTests : public QObject {
    Q_OBJECT

    private slots:
        void parse_returnsSummaryText_onValidGeminiResponse();
        void parse_returnsError_onInvalidJson();
        void parse_returnsError_whenCandidatesMissing();
        void parse_returnsError_whenTextPartsMissing();
        void parse_returnsError_whenSummaryTextEmpty();
};

void GeminiResponseParserTests::parse_returnsSummaryText_onValidGeminiResponse() {
    providers::GeminiResponseParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": [
                  {
                    "text": "Please fabricate crown for tooth #4."
                  }
                ]
              }
            }
          ]
        }
    )json";

    const auto response = parser.parse(responseBytes);

    QVERIFY(response.success());
    QVERIFY(response.isValid());
    QCOMPARE(response.summaryText(), QString("Please fabricate crown for tooth #4."));
    QVERIFY(response.errorMessage().isEmpty());
}

void GeminiResponseParserTests::parse_returnsError_onInvalidJson() {
    providers::GeminiResponseParser parser;

    const QByteArray responseBytes = "not-json";
    const auto response = parser.parse(responseBytes);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini response was not a JSON object."));
}

void GeminiResponseParserTests::parse_returnsError_whenCandidatesMissing() {
    providers::GeminiResponseParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": []
        }
    )json";

    const auto response = parser.parse(responseBytes);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini response contained no candidates."));
}

void GeminiResponseParserTests::parse_returnsError_whenTextPartsMissing() {
    providers::GeminiResponseParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": []
              }
            }
          ]
        }
    )json";

    const auto response = parser.parse(responseBytes);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini response contained no text parts."));
}

void GeminiResponseParserTests::parse_returnsError_whenSummaryTextEmpty() {
    providers::GeminiResponseParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": [
                  {
                    "text": "   "
                  }
                ]
              }
            }
          ]
        }
    )json";

    const auto response = parser.parse(responseBytes);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini response text was empty."));
}

QTEST_APPLESS_MAIN(GeminiResponseParserTests)

#include "GeminiResponseParserTests.moc"