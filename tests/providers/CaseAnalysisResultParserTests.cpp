#include <QtTest>

#include "infrastructure/providers/CaseAnalysisResultParser.hpp"

using icodental::infrastructure::providers::CaseAnalysisResultParser;

class CaseAnalysisResultParserTests : public QObject {
  Q_OBJECT
  private slots:
      void parse_returnsSummaryText_onValidStructuredGeminiResponse();
      void parse_returnsError_onInvalidEnvelopeJson();
      void parse_returnsError_whenCandidatesMissing();
      void parse_returnsError_whenStructuredTextIsNotJson();
      void parse_usesStructuredText_whenRawProviderTextMissing();
};

void CaseAnalysisResultParserTests::parse_returnsSummaryText_onValidStructuredGeminiResponse() {
    CaseAnalysisResultParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": [
                  {
                    "text": "{\
                    \"doctorName\":\"Dr. Kim\",\
                    \"officeName\":\"Onion Dental\",\
                    \"patientName\":\"Mumen Sarwa\",\
                    \"caseType\":\"Crown\",\
                    \"toothNumber\":\"#4\",\
                    \"shade\":\"A2\",\
                    \"specialInstructions\":\"Please refabricate crown on tooth #4.\",\
                    \"confidenceNote\":\"Handwriting mostly legible.\",\
                    \"rawProviderText\":\"Please refabricate crown on tooth #4.\"\
                    }"
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
    QCOMPARE(response.summaryText(), QString("Please refabricate crown on tooth #4."));
    QVERIFY(response.errorMessage().isEmpty());
}

void CaseAnalysisResultParserTests::parse_returnsError_onInvalidEnvelopeJson() {
    CaseAnalysisResultParser parser;

    const QByteArray responseBytes = "not-json";
    const auto response = parser.parse(responseBytes);

    QVERIFY(!response.success());
    QVERIFY(!response.isValid());
    QCOMPARE(response.errorMessage(), QString("Gemini response was not a JSON object."));
}

void CaseAnalysisResultParserTests::parse_returnsError_whenCandidatesMissing() {
    CaseAnalysisResultParser parser;

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

void CaseAnalysisResultParserTests::parse_returnsError_whenStructuredTextIsNotJson() {
    CaseAnalysisResultParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": [
                  {
                    "text": "This is not JSON."
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
    QCOMPARE(response.errorMessage(), QString("Gemini result text was not valid JSON."));
}

void CaseAnalysisResultParserTests::parse_usesStructuredText_whenRawProviderTextMissing() {
    CaseAnalysisResultParser parser;

    const QByteArray responseBytes = R"json(
        {
          "candidates": [
            {
              "content": {
                "parts": [
                  {
                    "text": "{\
                    \"doctorName\":\"Dr. Kim\",\
                    \"officeName\":\"Onion Dental\",\
                    \"patientName\":\"Mumen Sarwa\",\
                    \"caseType\":\"Crown\",\
                    \"toothNumber\":\"#4\",\
                    \"shade\":\"A2\",\
                    \"specialInstructions\":\"Please refabricate crown on tooth #4.\",\
                    \"confidenceNote\":\"Handwriting mostly legible.\",\
                    \"rawProviderText\":\"\"\
                    }"
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
    QVERIFY(response.summaryText().contains(QString("\"doctorName\":\"Dr. Kim\"")));
    QVERIFY(response.errorMessage().isEmpty());
}

QTEST_APPLESS_MAIN(CaseAnalysisResultParserTests)

#include "CaseAnalysisResultParserTests.moc"