#pragma once

#include "application/AnalysisPlan.hpp"
#include "application/AnalysisRequest.hpp"
#include "infrastructure/cache/AnalysisCacheRepository.hpp"
#include "infrastructure/filesystem/FileScanner.hpp"
#include "infrastructure/filesystem/ImageHasher.hpp"

namespace icodental::application {
    class AnalysisOrchestrator {
        public:
            AnalysisOrchestrator(
                infrastructure::filesystem::FileScanner fileScanner,
                infrastructure::filesystem::ImageHasher imageHasher,
                infrastructure::cache::AnalysisCacheRepository cacheRepository);

            [[nodiscard]] AnalysisPlan buildPlan(const AnalysisRequest& request) const;

        private:
            [[nodiscard]] QList<QString> resolveInputFiles(const AnalysisRequest& request) const;

            infrastructure::filesystem::FileScanner m_fileScanner;
            infrastructure::filesystem::ImageHasher m_imageHasher;
            infrastructure::cache::AnalysisCacheRepository m_cacheRepository;
    };
}