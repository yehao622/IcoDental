#include "ui/viewmodels/MainViewModel.hpp"

#include "application/AnalysisItem.hpp"
#include "application/AnalysisOrchestrator.hpp"
#include "application/AnalysisPlan.hpp"
#include "application/AnalysisRequest.hpp"
#include "domain/ProviderType.hpp"

namespace icodental::ui {
    MainViewModel::MainViewModel(
        icodental::application::AnalysisOrchestrator& analysisOrchestrator)
        : m_analysisOrchestrator(analysisOrchestrator)
    {}

    MainViewModel::AnalyzeOutcome MainViewModel::analyzeSingleImage(
        const QString& imagePath,
        const QString& providerName,
        const QString& model,
        bool forceRefresh) const
    {
        const auto provider = providerFromName(providerName);

        if (provider == icodental::domain::ProviderType::Unknown) {
            return {
                AnalyzeStatus::Error,
                "Choose a supported provider before analyzing.",
                std::nullopt
            };
        }

        const icodental::application::AnalysisRequest request(
            icodental::application::AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {imagePath},
            provider,
            model,
            forceRefresh);

        const auto plan = m_analysisOrchestrator.buildPlan(request);

        if (plan.hasErrors()) {
            return {
                AnalyzeStatus::Error,
                plan.errors().join("\n"),
                std::nullopt
            };
        }

        if (!plan.cachedItems().isEmpty()) {
            const auto& cachedItem = plan.cachedItems().first();

            if (!cachedItem.hasCachedEntry()
                || !cachedItem.cachedEntry().has_value()) {
                return {
                    AnalyzeStatus::Error,
                    "The cache plan contained an invalid cached entry.",
                    std::nullopt
                };
            }

            const auto& entry = cachedItem.cachedEntry().value();

            if (entry.provider() == provider && entry.model() == model) {
                return {
                    AnalyzeStatus::CacheHit,
                    "Loaded the existing cached analysis.",
                    entry.caseAnalysisResult()
                };
            }

            return {
                AnalyzeStatus::ProviderAnalysisRequired,
                "A cached result exists for this image, but it was created with a different provider or model.",
                std::nullopt
            };
        }

        if (!plan.pendingItems().isEmpty()) {
            return {
                AnalyzeStatus::ProviderAnalysisRequired,
                forceRefresh
                    ? "Force refresh is enabled. A new provider analysis is required."
                    : "No cached result exists for this image. A provider analysis is required.",
                std::nullopt
            };
        }

        return {
            AnalyzeStatus::Error,
            "No analysis item was produced for the selected image.",
            std::nullopt
        };
    }

    icodental::domain::ProviderType MainViewModel::providerFromName(
        const QString& providerName) {
        if (providerName.compare("Gemini", Qt::CaseInsensitive) == 0) {
            return icodental::domain::ProviderType::Gemini;
        }

        if (providerName.compare("Ollama", Qt::CaseInsensitive) == 0) {
            return icodental::domain::ProviderType::Ollama;
        }

        return icodental::domain::ProviderType::Unknown;
    }
}