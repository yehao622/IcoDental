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
      void parseStructuredText_acceptsFencedJson();
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

    QVERIFY(response.hasCaseAnalysisResult());
    QVERIFY(response.caseAnalysisResult().has_value());

    QVERIFY(response.isValid());
    QCOMPARE(response.summaryText(), QString("Please refabricate crown on tooth #4."));
    QVERIFY(response.errorMessage().isEmpty());

    const auto& result = response.caseAnalysisResult().value();
    QCOMPARE(result.doctorName(), QString("Dr. Kim"));
    QCOMPARE(result.officeName(), QString("Onion Dental"));
    QCOMPARE(result.patientName(), QString("Mumen Sarwa"));
    QCOMPARE(result.caseType(), QString("Crown"));
    QCOMPARE(result.toothNumber(), QString("#4"));
    QCOMPARE(result.shade(), QString("A2"));
    QCOMPARE(result.specialInstructions(), QString("Please refabricate crown on tooth #4."));
    QCOMPARE(result.confidenceNote(), QString("Handwriting mostly legible."));
    QCOMPARE(result.rawProviderText(), QString("Please refabricate crown on tooth #4."));
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
    QCOMPARE(response.errorMessage(), QString("Analysis result text was not valid JSON."));
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
    QVERIFY(response.hasCaseAnalysisResult());
    QVERIFY(response.caseAnalysisResult().has_value());

    const auto& result = response.caseAnalysisResult().value();
    QCOMPARE(result.doctorName(), QString("Dr. Kim"));
    QCOMPARE(result.rawProviderText(), response.summaryText());
}

void CaseAnalysisResultParserTests::parseStructuredText_acceptsFencedJson() {
    CaseAnalysisResultParser parser;

    const QString structuredText = R"(
      ```json
      {
        "doctorName": "Dr. Kim",
        "officeName": "Onion Dental",
        "patientName": "Mumen Sarwa",
        "caseType": "Crown",
        "toothNumber": "#4",
        "shade": "A2",
        "specialInstructions": "Refabricate crown.",
        "confidenceNote": "Legible.",
        "rawProviderText": "Refabricate crown."
      }
      ```
      )";

    const auto response = parser.parseStructuredText(
        structuredText,
        "{\"provider\":\"test\"}");

    QVERIFY(response.success());
    QVERIFY(response.hasCaseAnalysisResult());
    QVERIFY(response.caseAnalysisResult().has_value());

    const auto& result = response.caseAnalysisResult().value();
    QCOMPARE(result.doctorName(), QString("Dr. Kim"));
    QCOMPARE(result.rawProviderText(), QString("Refabricate crown."));
}

QTEST_APPLESS_MAIN(CaseAnalysisResultParserTests)

#include "CaseAnalysisResultParserTests.moc"