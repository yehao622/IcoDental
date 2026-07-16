#include "application/AnalysisItem.hpp"

#include <utility>

namespace icodental::application {
    AnalysisItem::AnalysisItem(
        QString filePath,
        domain::ImageFingerprint fingerprint,
        std::optional<infrastructure::cache::AnalysisCacheEntry> cachedEntry)
        : m_filePath(std::move(filePath))
        , m_fingerprint(std::move(fingerprint))
        , m_cachedEntry(std::move(cachedEntry))
    {}

    const QString& AnalysisItem::filePath() const {
        return m_filePath;
    }

    const domain::ImageFingerprint& AnalysisItem::fingerprint() const {
        return m_fingerprint;
    }

    const std::optional<infrastructure::cache::AnalysisCacheEntry>& AnalysisItem::cachedEntry() const {
        return m_cachedEntry;
    }

    bool AnalysisItem::hasCachedEntry() const {
        return m_cachedEntry.has_value();
    }
}