#include "ui/viewmodels/BatchAnalysisController.hpp"

#include <utility>

namespace icodental::ui {
    BatchAnalysisController::BatchAnalysisController(QObject* parent)
        : QObject(parent) {
        connect(
            &m_providerWatcher,
            &QFutureWatcher<ProviderResponse>::finished,
            this,
            &BatchAnalysisController::onProviderFinished);
    }

    void BatchAnalysisController::start(
        QList<BatchAnalysisItem> items,
        ProviderTask providerTask) {
        if (m_running) {
            emit batchFailedToStart(
                "A batch analysis is already running.");
            return;
        }

        if (items.isEmpty()) {
            emit batchFailedToStart(
                "No images were supplied for batch analysis.");
            return;
        }

        if (!providerTask) {
            emit batchFailedToStart(
                "No provider task was configured for batch analysis.");
            return;
        }

        m_items = std::move(items);
        m_providerTask = std::move(providerTask);
        m_currentItemIndex = -1;
        m_cancelRequested = false;
        m_running = true;

        emit batchStarted(totalCount());
        emit progressChanged(completedCount(), totalCount());

        startNextPendingItem();
    }

    void BatchAnalysisController::cancel() {
        if (!m_running) {
            return;
        }

        m_cancelRequested = true;
    }

    const QList<BatchAnalysisItem>& BatchAnalysisController::items() const {
        return m_items;
    }

    bool BatchAnalysisController::isRunning() const {
        return m_running;
    }

    int BatchAnalysisController::completedCount() const {
        return countItemsInState(BatchItemState::Cached)
            + countItemsInState(BatchItemState::Succeeded)
            + countItemsInState(BatchItemState::Failed)
            + countItemsInState(BatchItemState::Cancelled);
    }

    int BatchAnalysisController::totalCount() const {
        return m_items.size();
    }

    void BatchAnalysisController::onProviderFinished() {
        if (m_currentItemIndex < 0
            || m_currentItemIndex >= m_items.size()) {
            finishBatch();
            return;
        }

        BatchAnalysisItem& currentItem =
            m_items[m_currentItemIndex];

        const ProviderResponse response =
            m_providerWatcher.result();

        if (response.success()
            && response.hasCaseAnalysisResult()
            && response.caseAnalysisResult().has_value()) {
            currentItem.state = BatchItemState::Succeeded;
            currentItem.result = response.caseAnalysisResult();
            currentItem.message = "Analysis completed.";
        } else {
            currentItem.state = BatchItemState::Failed;
            currentItem.message =
                response.errorMessage().trimmed().isEmpty()
                    ? "Provider returned no structured analysis result."
                    : response.errorMessage();
        }

        emit itemUpdated(m_currentItemIndex);
        emit progressChanged(completedCount(), totalCount());

        m_currentItemIndex = -1;

        if (m_cancelRequested) {
            markPendingItemsCancelled();
            finishBatch();
            return;
        }

        startNextPendingItem();
    }

    void BatchAnalysisController::startNextPendingItem() {
        if (m_cancelRequested) {
            markPendingItemsCancelled();
            finishBatch();
            return;
        }

        const int nextIndex = findNextPendingIndex();

        if (nextIndex < 0) {
            finishBatch();
            return;
        }

        m_currentItemIndex = nextIndex;

        BatchAnalysisItem& nextItem = m_items[nextIndex];
        nextItem.state = BatchItemState::Running;
        nextItem.message = "Analyzing…";

        emit itemUpdated(nextIndex);

        m_providerWatcher.setFuture(
            m_providerTask(nextItem.imagePath, nextItem.fingerprint));
    }

    void BatchAnalysisController::finishBatch() {
        if (!m_running) {
            return;
        }

        m_running = false;
        m_currentItemIndex = -1;
        m_providerTask = {};

        const int succeededCount =
            countItemsInState(BatchItemState::Cached)
            + countItemsInState(BatchItemState::Succeeded);

        emit batchFinished(
            succeededCount,
            countItemsInState(BatchItemState::Failed),
            countItemsInState(BatchItemState::Cancelled));
    }

    void BatchAnalysisController::markPendingItemsCancelled() {
        for (int index = 0; index < m_items.size(); ++index) {
            BatchAnalysisItem& item = m_items[index];

            if (item.state != BatchItemState::Pending) {
                continue;
            }

            item.state = BatchItemState::Cancelled;
            item.message = "Cancelled before analysis started.";

            emit itemUpdated(index);
        }

        emit progressChanged(completedCount(), totalCount());
    }

    int BatchAnalysisController::findNextPendingIndex() const {
        for (int index = 0; index < m_items.size(); ++index) {
            if (m_items[index].state == BatchItemState::Pending) {
                return index;
            }
        }

        return -1;
    }

    int BatchAnalysisController::countItemsInState(
        BatchItemState state) const {
        int count = 0;

        for (const BatchAnalysisItem& item : m_items) {
            if (item.state == state) {
                ++count;
            }
        }

        return count;
    }
}