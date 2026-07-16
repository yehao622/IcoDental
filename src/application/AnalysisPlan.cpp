#include "application/AnalysisPlan.hpp"

#include <utility>

namespace icodental::application {
    AnalysisPlan::AnalysisPlan(
        QList<AnalysisItem> cachedItems,
        QList<AnalysisItem> pendingItems,
        QStringList errors)
        : m_cachedItems(std::move(cachedItems))
        , m_pendingItems(std::move(pendingItems))
        , m_errors(std::move(errors))
    {}

    const QList<AnalysisItem>& AnalysisPlan::cachedItems() const {
        return m_cachedItems;
    }

    const QList<AnalysisItem>& AnalysisPlan::pendingItems() const {
        return m_pendingItems;
    }

    const QStringList& AnalysisPlan::errors() const {
        return m_errors;
    }

    bool AnalysisPlan::hasErrors() const {
        return !m_errors.isEmpty();
    }

    bool AnalysisPlan::isEmpty() const {
        return m_cachedItems.isEmpty() && m_pendingItems.isEmpty();
    }
}