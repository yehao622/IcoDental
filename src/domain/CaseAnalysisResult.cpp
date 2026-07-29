#include "domain/CaseAnalysisResult.hpp"

namespace icodental::domain {
    CaseAnalysisResult::CaseAnalysisResult(
        QString doctorName,
        QString officeName,
        QString patientName,
        QString caseType,
        QString toothNumber,
        QString shade,
        QString specialInstructions,
        QString confidenceNote,
        QString rawProviderText)
        : m_doctorName(std::move(doctorName))
        , m_officeName(std::move(officeName))
        , m_patientName(std::move(patientName))
        , m_caseType(std::move(caseType))
        , m_toothNumber(std::move(toothNumber))
        , m_shade(std::move(shade))
        , m_specialInstructions(std::move(specialInstructions))
        , m_confidenceNote(std::move(confidenceNote))
        , m_rawProviderText(std::move(rawProviderText))
    {}

    bool CaseAnalysisResult::isValid() const {
        return !m_rawProviderText.trimmed().isEmpty();
    }

    const QString& CaseAnalysisResult::doctorName() const {
        return m_doctorName;
    }

    const QString& CaseAnalysisResult::officeName() const {
        return m_officeName;
    }

    const QString& CaseAnalysisResult::patientName() const {
        return m_patientName;
    }

    const QString& CaseAnalysisResult::caseType() const {
        return m_caseType;
    }

    const QString& CaseAnalysisResult::toothNumber() const {
        return m_toothNumber;
    }

    const QString& CaseAnalysisResult::shade() const {
        return m_shade;
    }

    const QString& CaseAnalysisResult::specialInstructions() const {
        return m_specialInstructions;
    }

    const QString& CaseAnalysisResult::confidenceNote() const {
        return m_confidenceNote;
    }

    const QString& CaseAnalysisResult::rawProviderText() const {
        return m_rawProviderText;
    }

    QJsonObject CaseAnalysisResult::toJson() const {
        return QJsonObject{
            {"doctorName", m_doctorName},
            {"officeName", m_officeName},
            {"patientName", m_patientName},
            {"caseType", m_caseType},
            {"toothNumber", m_toothNumber},
            {"shade", m_shade},
            {"specialInstructions", m_specialInstructions},
            {"confidenceNote", m_confidenceNote},
            {"rawProviderText", m_rawProviderText}
        };
    }

    CaseAnalysisResult CaseAnalysisResult::fromJson(const QJsonObject& json) {
        return CaseAnalysisResult(
            json.value("doctorName").toString(),
            json.value("officeName").toString(),
            json.value("patientName").toString(),
            json.value("caseType").toString(),
            json.value("toothNumber").toString(),
            json.value("shade").toString(),
            json.value("specialInstructions").toString(),
            json.value("confidenceNote").toString(),
            json.value("rawProviderText").toString());
    }
}