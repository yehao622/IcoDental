#pragma once

#include <QDateTime>
#include <QString>

#include "domain/ImageFingerprint.hpp"
#include "domain/ProviderType.hpp"

namespace icodental::infrastructure::cache {
    class AnalysisCacheEntry {
    public:
        AnalysisCacheEntry() = default;

        AnalysisCacheEntry(
            icodental::domain::ImageFingerprint fingerprint,
            icodental::domain::ProviderType provider,
            QString model,
            QString summaryText,
            QDateTime createdAtUtc,
            QDateTime updatedAtUtc);

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] const icodental::domain::ImageFingerprint& fingerprint() const;
        [[nodiscard]] icodental::domain::ProviderType provider() const;
        [[nodiscard]] const QString& model() const;
        [[nodiscard]] const QString& summaryText() const;
        [[nodiscard]] const QDateTime& createdAtUtc() const;
        [[nodiscard]] const QDateTime& updatedAtUtc() const;

    private:
        icodental::domain::ImageFingerprint m_fingerprint;
        icodental::domain::ProviderType m_provider{icodental::domain::ProviderType::Unknown};
        QString m_model;
        QString m_summaryText;
        QDateTime m_createdAtUtc;
        QDateTime m_updatedAtUtc;
    };
}