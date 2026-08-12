#pragma once

#include <QByteArray>

#include "domain/CaseAnalysisResult.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"

namespace icodental::infrastructure::providers {
    class CaseAnalysisResultParser {
        public:
            [[nodiscard]] ProviderResponse parse(const QByteArray& responseBytes) const;

        private:
            [[nodiscard]] icodental::domain::CaseAnalysisResult parseResultObject(
                    const QJsonObject& jsonObject,
                    const QString& structuredText) const;
    };
}