#include <QJsonObject>
#include <QtTest>

#include "domain/CaseAnalysisResult.hpp"

using icodental::domain::CaseAnalysisResult;

class CaseAnalysisResultTests : public QObject {
    Q_OBJECT
    private slots:
        void isValid_returnsFalse_whenRawProviderTextIsEmpty();
        void isValid_returnsTrue_whenRawProviderTextIsPresent();
        void toJson_returnsExpectedFields();
        void fromJson_restoresExpectedValues();
};

void CaseAnalysisResultTests::isValid_returnsFalse_whenRawProviderTextIsEmpty() {
    const CaseAnalysisResult result(
        "Dr. Smith",
        "Crystal Smile",
        "John Doe",
        "Crown",
        "#4",
        "A2",
        "Please fabricate crown for tooth #4.",
        "Readable with moderate confidence.",
        "   ");

    QVERIFY(!result.isValid());
}

void CaseAnalysisResultTests::isValid_returnsTrue_whenRawProviderTextIsPresent() {
    const CaseAnalysisResult result(
        "Dr. Smith",
        "Crystal Smile",
        "John Doe",
        "Crown",
        "#4",
        "A2",
        "Please fabricate crown for tooth #4.",
        "Readable with moderate confidence.",
        "Please fabricate crown for tooth #4.");

    QVERIFY(result.isValid());
}

void CaseAnalysisResultTests::toJson_returnsExpectedFields() {
    const CaseAnalysisResult result(
        "Dr. Smith",
        "Crystal Smile",
        "John Doe",
        "Crown",
        "#4",
        "A2",
        "Please fabricate crown for tooth #4.",
        "Readable with moderate confidence.",
        "Please fabricate crown for tooth #4.");

    const QJsonObject json = result.toJson();

    QCOMPARE(json.value("doctorName").toString(), QString("Dr. Smith"));
    QCOMPARE(json.value("officeName").toString(), QString("Crystal Smile"));
    QCOMPARE(json.value("patientName").toString(), QString("John Doe"));
    QCOMPARE(json.value("caseType").toString(), QString("Crown"));
    QCOMPARE(json.value("toothNumber").toString(), QString("#4"));
    QCOMPARE(json.value("shade").toString(), QString("A2"));
    QCOMPARE(json.value("specialInstructions").toString(), QString("Please fabricate crown for tooth #4."));
    QCOMPARE(json.value("confidenceNote").toString(), QString("Readable with moderate confidence."));
    QCOMPARE(json.value("rawProviderText").toString(), QString("Please fabricate crown for tooth #4."));
}

void CaseAnalysisResultTests::fromJson_restoresExpectedValues() {
    const QJsonObject json{
        {"doctorName", "Dr. Smith"},
        {"officeName", "Crystal Smile"},
        {"patientName", "John Doe"},
        {"caseType", "Crown"},
        {"toothNumber", "#4"},
        {"shade", "A2"},
        {"specialInstructions", "Please fabricate crown for tooth #4."},
        {"confidenceNote", "Readable with moderate confidence."},
        {"rawProviderText", "Please fabricate crown for tooth #4."}
    };

    const CaseAnalysisResult result = CaseAnalysisResult::fromJson(json);

    QCOMPARE(result.doctorName(), QString("Dr. Smith"));
    QCOMPARE(result.officeName(), QString("Crystal Smile"));
    QCOMPARE(result.patientName(), QString("John Doe"));
    QCOMPARE(result.caseType(), QString("Crown"));
    QCOMPARE(result.toothNumber(), QString("#4"));
    QCOMPARE(result.shade(), QString("A2"));
    QCOMPARE(result.specialInstructions(), QString("Please fabricate crown for tooth #4."));
    QCOMPARE(result.confidenceNote(), QString("Readable with moderate confidence."));
    QCOMPARE(result.rawProviderText(), QString("Please fabricate crown for tooth #4."));
    QVERIFY(result.isValid());
}

QTEST_APPLESS_MAIN(CaseAnalysisResultTests)

#include "CaseAnalysisResultTests.moc"