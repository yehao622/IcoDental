#pragma once

#include <memory>
#include <QString>

#include "infrastructure/providers/INetworkExecutor.hpp"
#include "infrastructure/providers/ProviderClient.hpp"

namespace icodental::infrastructure::providers {
    class GeminiClient : public ProviderClient {
        public:
            explicit GeminiClient(QString apiKey);
            GeminiClient(QString apiKey, INetworkExecutor* networkExecutor);
            ~GeminiClient() override = default;

            [[nodiscard]] ProviderResponse analyze(const ProviderRequest& request) override;

        private:
            [[nodiscard]] ProviderResponse buildErrorResponse(const QString& message) const;
            [[nodiscard]] QString buildEndpoint(const QString& model) const;

            QString m_apiKey;
            INetworkExecutor* m_networkExecutor{nullptr};
            std::unique_ptr<INetworkExecutor> m_ownedNetworkExecutor;
    };
}