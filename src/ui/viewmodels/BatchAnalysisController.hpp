#pragma once

#include <functional>
#include <optional>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QObject>
#include <QString>

#include "domain/CaseAnalysisResult.hpp"
#include "domain/ImageFingerprint.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"

namespace icodental::ui {
    enum class BatchItemState {
        Pending,
        Cached,
        Running,
        Succeeded,
        Failed,
        Cancelled
    };

    struct BatchAnalysisItem {
        QString imagePath;
        icodental::domain::ImageFingerprint fingerprint;
        BatchItemState state{BatchItemState::Pending};
        QString message;
        std::optional<icodental::domain::CaseAnalysisResult> result;
    };

    class BatchAnalysisController final : public QObject {
        Q_OBJECT
        public:
            using ProviderResponse = icodental::infrastructure::providers::ProviderResponse;
            using ProviderTask = std::function<QFuture<ProviderResponse>(
                const QString& imagePath,
                const icodental::domain::ImageFingerprint& fingerprint)>;

            explicit BatchAnalysisController(QObject* parent = nullptr);

            void start(
                QList<BatchAnalysisItem> items,
                ProviderTask providerTask);

            void cancel();

            [[nodiscard]] const QList<BatchAnalysisItem>& items() const;
            [[nodiscard]] bool isRunning() const;
            [[nodiscard]] int completedCount() const;
            [[nodiscard]] int totalCount() const;

        signals:
            void batchStarted(int totalCount);
            void itemUpdated(int index);
            void progressChanged(int completedCount, int totalCount);
            void batchFinished(
                int succeededCount,
                int failedCount,
                int cancelledCount);
            void batchFailedToStart(const QString& errorMessage);

        private slots:
            void onProviderFinished();

        private:
            void startNextPendingItem();
            void finishBatch();
            void markPendingItemsCancelled();

            [[nodiscard]] int findNextPendingIndex() const;
            [[nodiscard]] int countItemsInState(BatchItemState state) const;

            QList<BatchAnalysisItem> m_items;
            ProviderTask m_providerTask;
            QFutureWatcher<ProviderResponse> m_providerWatcher;
            int m_currentItemIndex{-1};
            bool m_cancelRequested{false};
            bool m_running{false};
    };
}