#pragma once

#include <QNetworkAccessManager>
#include <QString>

#include "infrastructure/providers/ProviderClient.hpp"

namespace icodental::infrastructure::providers {
    class GeminiClient : public ProviderClient {
        public:
            GeminiClient(QString apiKey, QNetworkAccessManager* networkAccessManager = nullptr);
            ~GeminiClient() override;

            [[nodiscard]] ProviderResponse analyze(const ProviderRequest& request) override;

        private:
            [[nodiscard]] ProviderResponse buildErrorResponse(const QString& message) const;

            QString m_apiKey;
            QNetworkAccessManager* m_networkAccessManager{nullptr};
            bool m_ownsNetworkAccessManager{false};
    };
}