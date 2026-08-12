#pragma once

#include <memory>

#include <QString>

#include "application/AnalysisOrchestrator.hpp"
#include "infrastructure/cache/AnalysisCacheRepository.hpp"
#include "infrastructure/cache/CacheDatabase.hpp"

namespace icodental::bootstrap {
    class ApplicationServices final {
        public:
            explicit ApplicationServices(QString cacheDatabasePath);

            ApplicationServices(const ApplicationServices&) = delete;
            ApplicationServices& operator=(const ApplicationServices&) = delete;
            ApplicationServices(ApplicationServices&&) = delete;
            ApplicationServices& operator=(ApplicationServices&&) = delete;

            [[nodiscard]] bool initialize();

            [[nodiscard]] bool isInitialized() const;
            [[nodiscard]] const QString& lastError() const;

            [[nodiscard]] application::AnalysisOrchestrator& analysisOrchestrator();
            [[nodiscard]] infrastructure::cache::AnalysisCacheRepository& cacheRepository();

            [[nodiscard]] static QString defaultCacheDatabasePath();

        private:
            infrastructure::cache::CacheDatabase m_cacheDatabase;
            std::unique_ptr<infrastructure::cache::AnalysisCacheRepository> m_cacheRepository;
            std::unique_ptr<application::AnalysisOrchestrator> m_analysisOrchestrator;

            QString m_lastError;
    };
}