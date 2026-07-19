#pragma once

#include <QByteArray>
#include <QJsonObject>

#include "infrastructure/providers/ProviderRequest.hpp"

namespace icodental::infrastructure::providers {
    class GeminiPayloadBuilder {
        public:
            [[nodiscard]] QJsonObject build(
                const ProviderRequest& request,
                const QByteArray& imageBytes) const;
    };
}