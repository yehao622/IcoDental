#include "ui/viewmodels/MainViewModel.hpp"

#include <QDateTime>
#include <QtConcurrent/QtConcurrentRun>

#include "application/AnalysisItem.hpp"
#include "application/AnalysisOrchestrator.hpp"
#include "application/AnalysisPlan.hpp"
#include "application/AnalysisRequest.hpp"
#include "infrastructure/cache/AnalysisCacheEntry.hpp"
#include "infrastructure/cache/AnalysisCacheRepository.hpp"
#include "infrastructure/config/SecretResolver.hpp"
#include "infrastructure/providers/GeminiClient.hpp"
#include "infrastructure/providers/ProviderRequest.hpp"

namespace icodental::ui {
    using icodental::application::AnalysisRequest;
    using icodental::domain::CaseAnalysisResult;
    using icodental::domain::ProviderType;
    using icodental::infrastructure::cache::AnalysisCacheEntry;
    using icodental::infrastructure::providers::GeminiClient;
    using icodental::infrastructure::providers::ProviderRequest;
    using icodental::infrastructure::providers::ProviderResponse;

    MainViewModel::MainViewModel(
        icodental::application::AnalysisOrchestrator& analysisOrchestrator,
        icodental::infrastructure::cache::AnalysisCacheRepository& cacheRepository,
        QObject* parent)
        : QObject(parent)
        , m_analysisOrchestrator(analysisOrchestrator)
        , m_cacheRepository(cacheRepository)
    {
        connect(
        &m_providerWatcher,
        &QFutureWatcher<ProviderResponse>::finished,
        this,
        &MainViewModel::onProviderFinished);
    }

    void MainViewModel::analyzeSingleImage(
        const QString& imagePath,
        const QString& providerName,
        const QString& model,
        const QString& optionalNote,
        bool forceRefresh) {
        if (m_providerWatcher.isRunning()) {
            emit analysisFailed("An analysis is already running.");
            return;
        }

        const ProviderType provider = providerFromName(providerName);

        if (provider == ProviderType::Unknown) {
            emit analysisFailed("Choose a supported provider before analyzing.");
            return;
        }

        const AnalysisRequest request(
            AnalysisRequest::InputMode::ExplicitFiles,
            QString(),
            {imagePath},
            provider,
            model,
            forceRefresh);

        const auto plan = m_analysisOrchestrator.buildPlan(request);

        if (plan.hasErrors()) {
            emit analysisFailed(plan.errors().join("\n"));
            return;
        }

        if (!plan.cachedItems().isEmpty()) {
            const auto& cachedItem = plan.cachedItems().first();

            if (!cachedItem.hasCachedEntry()
                || !cachedItem.cachedEntry().has_value()) {
                emit analysisFailed(
                    "The cache plan contained an invalid cached entry.");
                return;
            }

            const auto& cachedEntry = cachedItem.cachedEntry().value();

            if (cachedEntry.provider() == provider
                && cachedEntry.model() == model) {
                emit analysisSucceeded(
                    cachedEntry.caseAnalysisResult(),
                    "Loaded the existing cached analysis.");
                return;
            }

            startGeminiAnalysis(
                imagePath,
                cachedItem.fingerprint(),
                model,
                optionalNote);
            return;
        }

        if (!plan.pendingItems().isEmpty()) {
            const auto& pendingItem = plan.pendingItems().first();

            if (provider == ProviderType::Ollama) {
                emit analysisFailed(
                    "Ollama analysis is not implemented yet. Choose Gemini.");
                return;
            }

            startGeminiAnalysis(
                imagePath,
                pendingItem.fingerprint(),
                model,
                optionalNote);
            return;
        }

        emit analysisFailed(
            "No analysis item was produced for the selected image.");
    }

    void MainViewModel::onProviderFinished() {
        const ProviderResponse response = m_providerWatcher.result();

        if (!m_pendingAnalysis.has_value()) {
            emit analysisFailed(
                "Provider analysis completed without pending request state.");
            return;
        }

        const PendingAnalysis pending = std::move(m_pendingAnalysis.value());
        m_pendingAnalysis.reset();

        if (!response.success()) {
            emit analysisFailed(
                response.errorMessage().trimmed().isEmpty()
                    ? "Gemini analysis failed."
                    : response.errorMessage());
            return;
        }

        if (!response.hasCaseAnalysisResult()
            || !response.caseAnalysisResult().has_value()) {
            emit analysisFailed(
                "Gemini returned no structured case analysis result.");
            return;
        }

        const CaseAnalysisResult result =
            response.caseAnalysisResult().value();

        const QDateTime now = QDateTime::currentDateTimeUtc();

        const AnalysisCacheEntry cacheEntry(
            pending.fingerprint,
            pending.provider,
            pending.model,
            response.summaryText(),
            result,
            now,
            now);

        if (!m_cacheRepository.save(cacheEntry)) {
            emit analysisSucceeded(
                result,
                QString(
                    "Analysis completed, but the result could not be saved to cache: %1")
                    .arg(m_cacheRepository.lastError()));
            return;
        }

        emit analysisSucceeded(
            result,
            "Analysis completed and saved to the local cache.");
    }

    ProviderType MainViewModel::providerFromName(
        const QString& providerName) {
        if (providerName.compare("Gemini", Qt::CaseInsensitive) == 0) {
            return ProviderType::Gemini;
        }

        if (providerName.compare("Ollama", Qt::CaseInsensitive) == 0) {
            return ProviderType::Ollama;
        }

        return ProviderType::Unknown;
    }

    QString MainViewModel::buildGeminiPrompt(
        const QString& optionalNote) {
        QString prompt =
            "Read this dental prescription image. "
            "Return only one valid JSON object with these string properties: "
            "doctorName, officeName, patientName, caseType, toothNumber, shade, "
            "specialInstructions, confidenceNote, rawProviderText. "
            "Use an empty string for unavailable values. "
            "Do not add Markdown, explanation, or code fences.";

        if (!optionalNote.trimmed().isEmpty()) {
            prompt += "\n\nAdditional user instruction:\n"
                + optionalNote.trimmed();
        }

        return prompt;
    }

    void MainViewModel::startGeminiAnalysis(
        const QString& imagePath,
        const icodental::domain::ImageFingerprint& fingerprint,
        const QString& model,
        const QString& optionalNote) {
        const QString apiKey =
            icodental::infrastructure::config::SecretResolver::geminiApiKey();

        if (apiKey.trimmed().isEmpty()) {
            emit analysisFailed(
                "Gemini API key is missing. Set GEMINI_API_KEY or GOOGLE_API_KEY "
                "in the terminal before launching IcoDental.");
            return;
        }

        const ProviderRequest request(
            ProviderType::Gemini,
            model,
            buildGeminiPrompt(optionalNote),
            imagePath);

        if (!request.isValid()) {
            emit analysisFailed("The Gemini provider request is invalid.");
            return;
        }

        m_pendingAnalysis = PendingAnalysis{
            fingerprint,
            ProviderType::Gemini,
            model
        };

        emit analysisStarted("Analyzing with Gemini…");

        m_providerWatcher.setFuture(
            QtConcurrent::run(
                [apiKey, request]() {
                    GeminiClient client(apiKey);
                    return client.analyze(request);
                }));
    }
}