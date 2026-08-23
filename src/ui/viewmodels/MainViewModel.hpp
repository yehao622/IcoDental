#pragma once

#include <optional>

#include <QFutureWatcher>
#include <QObject>
#include <QString>

#include "domain/CaseAnalysisResult.hpp"
#include "domain/ImageFingerprint.hpp"
#include "domain/ProviderType.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"
#include "ui/viewmodels/BatchAnalysisController.hpp"

namespace icodental::application {
    class AnalysisOrchestrator;
}

namespace icodental::infrastructure::cache {
    class AnalysisCacheRepository;
}

namespace icodental::ui {
    class MainViewModel final : public QObject {
        Q_OBJECT
        public:
            explicit MainViewModel(icodental::application::AnalysisOrchestrator& analysisOrchestrator,
                icodental::infrastructure::cache::AnalysisCacheRepository& cacheRepository,
                QObject* parent = nullptr);

            void analyzeSingleImage(
                const QString& imagePath,
                const QString& providerName,
                const QString& model,
                const QString& optionalNote,
                bool forceRefresh);

            void analyzeBatch(
                const QStringList& imagePaths,
                const QString& providerName,
                const QString& model,
                const QString& optionalNote,
                bool forceRefresh);

            void cancelBatchAnalysis();
            [[nodiscard]] const BatchAnalysisController& batchController() const;

        signals:
            void analysisStarted(const QString& message);

            void analysisSucceeded(
                const icodental::domain::CaseAnalysisResult& result,
                const QString& message);

            void analysisFailed(const QString& message);

            void batchAnalysisStarted(int totalCount);

            void batchAnalysisItemUpdated(int index);

            void batchAnalysisProgressChanged(
                int completedCount,
                int totalCount);

            void batchAnalysisFinished(
                int succeededCount,
                int failedCount,
                int cancelledCount);

            void batchAnalysisFailedToStart(const QString& errorMessage);

        private slots:
            void onProviderFinished();

        private:
            struct PendingAnalysis {
                icodental::domain::ImageFingerprint fingerprint;
                icodental::domain::ProviderType provider;
                QString model;
            };

            [[nodiscard]] static icodental::domain::ProviderType providerFromName(const QString& providerName);
            [[nodiscard]] static QString buildAnalysisPrompt(const QString& optionalNote);
            [[nodiscard]] QFuture<icodental::infrastructure::providers::ProviderResponse> startProviderTask(
                icodental::domain::ProviderType provider,
                const QString& model,
                const QString& optionalNote,
                const QString& imagePath);

            void startOllamaAnalysis(
                const QString& imagePath,
                const icodental::domain::ImageFingerprint& fingerprint,
                const QString& model,
                const QString& optionalNote);

            void startGeminiAnalysis(
                const QString& imagePath,
                const icodental::domain::ImageFingerprint& fingerprint,
                const QString& model,
                const QString& optionalNote);

            icodental::application::AnalysisOrchestrator& m_analysisOrchestrator;
            icodental::infrastructure::cache::AnalysisCacheRepository& m_cacheRepository;

            QFutureWatcher<icodental::infrastructure::providers::ProviderResponse> m_providerWatcher;
            std::optional<PendingAnalysis> m_pendingAnalysis;
            BatchAnalysisController m_batchController;
    };
}