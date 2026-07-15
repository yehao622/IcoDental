#pragma once

#include <optional>
#include <QString>
#include <QSqlDatabase>

#include "domain/ImageFingerprint.hpp"
#include "infrastructure/cache/AnalysisCacheEntry.hpp"

namespace icodental::infrastructure::cache {
    class AnalysisCacheRepository {
        public:
            explicit AnalysisCacheRepository(QSqlDatabase database);

            [[nodiscard]] std::optional<AnalysisCacheEntry> findByFingerprint(
                const icodental::domain::ImageFingerprint& fingerprint) const;

            [[nodiscard]] bool save(const AnalysisCacheEntry& entry);
            [[nodiscard]] QString lastError() const;

        private:
            [[nodiscard]] static QString providerToString(icodental::domain::ProviderType provider);
            [[nodiscard]] static icodental::domain::ProviderType providerFromString(const QString& value);

            QSqlDatabase m_database;
            QString m_lastError;
        };
}