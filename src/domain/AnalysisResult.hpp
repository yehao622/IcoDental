#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QString>

#include "ProviderType.hpp"

namespace icodental::domain {

class AnalysisResult {
    public:
        [[nodiscard]] const QString& patientName() const;
        void setPatientName(QString value);

        [[nodiscard]] const QString& doctorName() const;
        void setDoctorName(QString value);

        [[nodiscard]] const QString& officeName() const;
        void setOfficeName(QString value);

        [[nodiscard]] const QString& todayDate() const;
        void setTodayDate(QString value);

        [[nodiscard]] const QString& dueDate() const;
        void setDueDate(QString value);

        [[nodiscard]] const QString& specialInstructionsRaw() const;
        void setSpecialInstructionsRaw(QString value);

        [[nodiscard]] const QString& specialInstructionsNormalized() const;
        void setSpecialInstructionsNormalized(QString value);

        [[nodiscard]] double confidence() const;
        void setConfidence(double value);

        [[nodiscard]] bool needsReview() const;
        void setNeedsReview(bool value);

        [[nodiscard]] ProviderType providerType() const;
        void setProviderType(ProviderType value);

        [[nodiscard]] const QString& modelName() const;
        void setModelName(QString value);

        [[nodiscard]] const QString& promptVersion() const;
        void setPromptVersion(QString value);

        [[nodiscard]] QJsonObject toJson() const;
        static AnalysisResult fromJson(const QJsonObject& json);

    private:
        QString m_patientName;
        QString m_doctorName;
        QString m_officeName;
        QString m_todayDate;
        QString m_dueDate;
        QString m_specialInstructionsRaw;
        QString m_specialInstructionsNormalized;
        double m_confidence = 0.0;
        bool m_needsReview = true;
        ProviderType m_providerType = ProviderType::Unknown;
        QString m_modelName;
        QString m_promptVersion;
    };

} // namespace icodental::domain