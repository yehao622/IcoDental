#include "infrastructure/cache/AnalysisCacheEntry.hpp"

#include <utility>

namespace icodental::infrastructure::cache {
    using icodental::domain::ImageFingerprint;
    using icodental::domain::ProviderType;

    AnalysisCacheEntry::AnalysisCacheEntry(
        ImageFingerprint fingerprint,
        ProviderType provider,
        QString model,
        QString summaryText,
        QDateTime createdAtUtc,
        QDateTime updatedAtUtc)
        : m_fingerprint(std::move(fingerprint))
        , m_provider(provider)
        , m_model(std::move(model))
        , m_summaryText(std::move(summaryText))
        , m_createdAtUtc(std::move(createdAtUtc))
        , m_updatedAtUtc(std::move(updatedAtUtc))
    {}

    bool AnalysisCacheEntry::isValid() const {
        return m_fingerprint.isValid()
            && m_provider != ProviderType::Unknown
            && !m_model.trimmed().isEmpty()
            && !m_summaryText.trimmed().isEmpty()
            && m_createdAtUtc.isValid()
            && m_updatedAtUtc.isValid();
    }

    const ImageFingerprint& AnalysisCacheEntry::fingerprint() const { return m_fingerprint; }
    ProviderType AnalysisCacheEntry::provider() const { return m_provider; }
    const QString& AnalysisCacheEntry::model() const { return m_model; }
    const QString& AnalysisCacheEntry::summaryText() const { return m_summaryText; }
    const QDateTime& AnalysisCacheEntry::createdAtUtc() const { return m_createdAtUtc; }
    const QDateTime& AnalysisCacheEntry::updatedAtUtc() const { return m_updatedAtUtc; }
}