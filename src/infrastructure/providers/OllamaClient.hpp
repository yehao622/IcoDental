#pragma once

#include <memory>
#include <QString>

#include "infrastructure/providers/INetworkExecutor.hpp"
#include "infrastructure/providers/ProviderClient.hpp"

namespace icodental::infrastructure::providers {
    class OllamaClient final : public ProviderClient {
        public:
            OllamaClient(QString baseUrl, QString model);
            OllamaClient(
                QString baseUrl,
                QString model,
                INetworkExecutor* networkExecutor);
            ~OllamaClient() override = default;

            [[nodiscard]] ProviderResponse analyze(const ProviderRequest& request) override;

        private:
            [[nodiscard]] ProviderResponse buildErrorResponse(const QString& message) const;
            [[nodiscard]] QUrl buildEndpoint() const;

            QString m_baseUrl;
            QString m_model;
            std::unique_ptr<INetworkExecutor> m_ownedNetworkExecutor;
            INetworkExecutor* m_networkExecutor{nullptr};
    };
}