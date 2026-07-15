#include "infrastructure/cache/AnalysisCacheRepository.hpp"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace icodental::infrastructure::cache {

    using icodental::domain::ImageFingerprint;
    using icodental::domain::ProviderType;

    AnalysisCacheRepository::AnalysisCacheRepository(QSqlDatabase database)
        : m_database(std::move(database))
    {}

    std::optional<AnalysisCacheEntry> AnalysisCacheRepository::findByFingerprint(
        const ImageFingerprint& fingerprint) const {
        if (!fingerprint.isValid() || !m_database.isOpen()) {
            return std::nullopt;
        }

        QSqlQuery query(m_database);
        query.prepare(R"(
            SELECT fingerprint, provider, model, summary_text, created_at_utc, updated_at_utc
            FROM analysis_cache
            WHERE fingerprint = :fingerprint
        )");
        query.bindValue(":fingerprint", fingerprint.sha256());

        if (!query.exec() || !query.next()) {
            return std::nullopt;
        }

        return AnalysisCacheEntry(
            ImageFingerprint(query.value(0).toString()),
            providerFromString(query.value(1).toString()),
            query.value(2).toString(),
            query.value(3).toString(),
            QDateTime::fromString(query.value(4).toString(), Qt::ISODate),
            QDateTime::fromString(query.value(5).toString(), Qt::ISODate));
    }

    bool AnalysisCacheRepository::save(const AnalysisCacheEntry& entry) {
        if (!entry.isValid() || !m_database.isOpen()) {
            m_lastError = "Invalid cache entry or database is closed";
            return false;
        }

        QSqlQuery query(m_database);
        query.prepare(R"(
            INSERT INTO analysis_cache (
                fingerprint, provider, model, summary_text, created_at_utc, updated_at_utc
            ) VALUES (
                :fingerprint, :provider, :model, :summary_text, :created_at_utc, :updated_at_utc
            )
            ON CONFLICT(fingerprint) DO UPDATE SET
                provider = excluded.provider,
                model = excluded.model,
                summary_text = excluded.summary_text,
                updated_at_utc = excluded.updated_at_utc
        )");

        query.bindValue(":fingerprint", entry.fingerprint().sha256());
        query.bindValue(":provider", providerToString(entry.provider()));
        query.bindValue(":model", entry.model());
        query.bindValue(":summary_text", entry.summaryText());
        query.bindValue(":created_at_utc", entry.createdAtUtc().toUTC().toString(Qt::ISODate));
        query.bindValue(":updated_at_utc", entry.updatedAtUtc().toUTC().toString(Qt::ISODate));

        if (!query.exec()) {
            m_lastError = query.lastError().text();
            return false;
        }

        return true;
    }

    QString AnalysisCacheRepository::lastError() const {
        return m_lastError;
    }

    QString AnalysisCacheRepository::providerToString(ProviderType provider) {
        switch (provider) {
            case ProviderType::Gemini:
                return "gemini";
            case ProviderType::Ollama:
                return "ollama";
            default:
                return "unknown";
        }
    }

    ProviderType AnalysisCacheRepository::providerFromString(const QString& value) {
        if (value == "gemini") {
            return ProviderType::Gemini;
        }

        if (value == "ollama") {
            return ProviderType::Ollama;
        }

        return ProviderType::Unknown;
    }
}