#include "AnalysisResult.hpp"

#include <algorithm>

namespace icodental::domain {

    const QString& AnalysisResult::patientName() const { return m_patientName; }
    void AnalysisResult::setPatientName(QString value) { m_patientName = std::move(value); }

    const QString& AnalysisResult::doctorName() const { return m_doctorName; }
    void AnalysisResult::setDoctorName(QString value) { m_doctorName = std::move(value); }

    const QString& AnalysisResult::officeName() const { return m_officeName; }
    void AnalysisResult::setOfficeName(QString value) { m_officeName = std::move(value); }

    const QString& AnalysisResult::todayDate() const { return m_todayDate; }
    void AnalysisResult::setTodayDate(QString value) { m_todayDate = std::move(value); }

    const QString& AnalysisResult::dueDate() const { return m_dueDate; }
    void AnalysisResult::setDueDate(QString value) { m_dueDate = std::move(value); }

    const QString& AnalysisResult::specialInstructionsRaw() const { return m_specialInstructionsRaw; }
    void AnalysisResult::setSpecialInstructionsRaw(QString value) { m_specialInstructionsRaw = std::move(value); }

    const QString& AnalysisResult::specialInstructionsNormalized() const { return m_specialInstructionsNormalized; }
    void AnalysisResult::setSpecialInstructionsNormalized(QString value) { m_specialInstructionsNormalized = std::move(value); }

    double AnalysisResult::confidence() const { return m_confidence; }
    void AnalysisResult::setConfidence(double value) { m_confidence = std::clamp(value, 0.0, 1.0); }

    bool AnalysisResult::needsReview() const { return m_needsReview; }
    void AnalysisResult::setNeedsReview(bool value) { m_needsReview = value; }

    ProviderType AnalysisResult::providerType() const { return m_providerType; }
    void AnalysisResult::setProviderType(ProviderType value) { m_providerType = value; }

    const QString& AnalysisResult::modelName() const { return m_modelName; }
    void AnalysisResult::setModelName(QString value) { m_modelName = std::move(value); }

    const QString& AnalysisResult::promptVersion() const { return m_promptVersion; }
    void AnalysisResult::setPromptVersion(QString value) { m_promptVersion = std::move(value); }

    QJsonObject AnalysisResult::toJson() const {
        return QJsonObject{
            {"patient_name", m_patientName},
            {"doctor_name", m_doctorName},
            {"office_name", m_officeName},
            {"today_date", m_todayDate},
            {"due_date", m_dueDate},
            {"special_instructions_raw", m_specialInstructionsRaw},
            {"special_instructions_normalized", m_specialInstructionsNormalized},
            {"confidence", m_confidence},
            {"needs_review", m_needsReview},
            {"provider_type", toString(m_providerType)},
            {"model_name", m_modelName},
            {"prompt_version", m_promptVersion}
        };
    }

    AnalysisResult AnalysisResult::fromJson(const QJsonObject& json) {
        AnalysisResult result;
        result.setPatientName(json.value("patient_name").toString());
        result.setDoctorName(json.value("doctor_name").toString());
        result.setOfficeName(json.value("office_name").toString());
        result.setTodayDate(json.value("today_date").toString());
        result.setDueDate(json.value("due_date").toString());
        result.setSpecialInstructionsRaw(json.value("special_instructions_raw").toString());
        result.setSpecialInstructionsNormalized(json.value("special_instructions_normalized").toString());
        result.setConfidence(json.value("confidence").toDouble(0.0));
        result.setNeedsReview(json.value("needs_review").toBool(true));

        const auto provider = providerTypeFromString(json.value("provider_type").toString());
        result.setProviderType(provider.value_or(ProviderType::Unknown));

        result.setModelName(json.value("model_name").toString());
        result.setPromptVersion(json.value("prompt_version").toString());
        return result;
    }
}