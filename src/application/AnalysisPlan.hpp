#pragma once

#include <QList>
#include <QStringList>

#include "application/AnalysisItem.hpp"

namespace icodental::application {
    class AnalysisPlan {
        public:
            AnalysisPlan() = default;
            
            AnalysisPlan(
                QList<AnalysisItem> cachedItems,
                QList<AnalysisItem> pendingItems,
                QStringList errors);

            [[nodiscard]] const QList<AnalysisItem>& cachedItems() const;
            [[nodiscard]] const QList<AnalysisItem>& pendingItems() const;
            [[nodiscard]] const QStringList& errors() const;

            [[nodiscard]] bool hasErrors() const;
            [[nodiscard]] bool isEmpty() const;

        private:
            QList<AnalysisItem> m_cachedItems;
            QList<AnalysisItem> m_pendingItems;
            QStringList m_errors;
    };
}