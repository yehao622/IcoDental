#pragma once

#include <optional>

#include <QString>

#include "domain/CaseAnalysisResult.hpp"
#include "domain/ProviderType.hpp"

namespace icodental::application {
    class AnalysisOrchestrator;
}

namespace icodental::ui {
    class MainViewModel final {
        public:
            enum class AnalyzeStatus {
                CacheHit,
                ProviderAnalysisRequired,
                Error
            };

            struct AnalyzeOutcome {
                AnalyzeStatus status{AnalyzeStatus::Error};
                QString message;
                std::optional<icodental::domain::CaseAnalysisResult> caseAnalysisResult;
            };

            explicit MainViewModel(icodental::application::AnalysisOrchestrator& analysisOrchestrator);

            [[nodiscard]] AnalyzeOutcome analyzeSingleImage(
                const QString& imagePath,
                const QString& providerName,
                const QString& model,
                bool forceRefresh) const;

        private:
            [[nodiscard]] static icodental::domain::ProviderType providerFromName(const QString& providerName);

            icodental::application::AnalysisOrchestrator& m_analysisOrchestrator;
    };
}