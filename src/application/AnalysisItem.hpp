#pragma once

#include <optional>

#include <QString>

#include "domain/ImageFingerprint.hpp"
#include "infrastructure/cache/AnalysisCacheEntry.hpp"

namespace icodental::application {
    class AnalysisItem {
        public:
            AnalysisItem(
                QString filePath,
                domain::ImageFingerprint fingerprint,
                std::optional<infrastructure::cache::AnalysisCacheEntry> cachedEntry);

            [[nodiscard]] const QString& filePath() const;
            [[nodiscard]] const domain::ImageFingerprint& fingerprint() const;
            [[nodiscard]] const std::optional<infrastructure::cache::AnalysisCacheEntry>& cachedEntry() const;

            [[nodiscard]] bool hasCachedEntry() const;

        private:
            QString m_filePath;
            domain::ImageFingerprint m_fingerprint;
            std::optional<infrastructure::cache::AnalysisCacheEntry> m_cachedEntry;
    };
}