#pragma once

#include <QJsonObject>
#include <QString>

namespace icodental::domain {
    class CaseAnalysisResult {
        public:
            CaseAnalysisResult() = default;

            CaseAnalysisResult(
                QString doctorName,
                QString officeName,
                QString patientName,
                QString caseType,
                QString toothNumber,
                QString shade,
                QString specialInstructions,
                QString confidenceNote,
                QString rawProviderText);

            [[nodiscard]] bool isValid() const;

            [[nodiscard]] const QString& doctorName() const;
            [[nodiscard]] const QString& officeName() const;
            [[nodiscard]] const QString& patientName() const;
            [[nodiscard]] const QString& caseType() const;
            [[nodiscard]] const QString& toothNumber() const;
            [[nodiscard]] const QString& shade() const;
            [[nodiscard]] const QString& specialInstructions() const;
            [[nodiscard]] const QString& confidenceNote() const;
            [[nodiscard]] const QString& rawProviderText() const;

            [[nodiscard]] QJsonObject toJson() const;
            static CaseAnalysisResult fromJson(const QJsonObject& json);

            friend bool operator==(const CaseAnalysisResult& lhs, const CaseAnalysisResult& rhs) = default;

        private:
            QString m_doctorName;
            QString m_officeName;
            QString m_patientName;
            QString m_caseType;
            QString m_toothNumber;
            QString m_shade;
            QString m_specialInstructions;
            QString m_confidenceNote;
            QString m_rawProviderText;
    };
}