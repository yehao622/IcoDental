#pragma once

#include <QJsonObject>
#include <QUrl>

#include "infrastructure/providers/INetworkExecutor.hpp"

namespace icodental::infrastructure::providers {
    class FakeNetworkExecutor : public INetworkExecutor {
        public:
            NetworkResult nextResult;
            QUrl lastUrl;
            QJsonObject lastPayload;
            int callCount{0};

            [[nodiscard]] NetworkResult postJson(
                const QUrl& url,
                const QJsonObject& payload) override
            {
                lastUrl = url;
                lastPayload = payload;
                ++callCount;
                return nextResult;
            }
    };
}