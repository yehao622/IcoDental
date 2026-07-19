#pragma once

#include <QByteArray>

#include "infrastructure/providers/ProviderResponse.hpp"

namespace icodental::infrastructure::providers {
    class GeminiResponseParser {
        public:
            [[nodiscard]] ProviderResponse parse(const QByteArray& responseBytes) const;
    };
}