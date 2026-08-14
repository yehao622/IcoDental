#include "infrastructure/providers/OllamaClient.hpp"

#include <utility>

#include <QUrl>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/QtNetworkExecutor.hpp"

namespace icodental::infrastructure::providers {
    OllamaClient::OllamaClient(QString baseUrl, QString model)
        : m_baseUrl(std::move(baseUrl))
        , m_model(std::move(model))
        , m_ownedNetworkExecutor(std::make_unique<QtNetworkExecutor>())
        , m_networkExecutor(m_ownedNetworkExecutor.get())
    {}

    OllamaClient::OllamaClient(
        QString baseUrl,
        QString model,
        INetworkExecutor* networkExecutor)
        : m_baseUrl(std::move(baseUrl))
        , m_model(std::move(model))
        , m_networkExecutor(networkExecutor)
    {}

    ProviderResponse OllamaClient::analyze(const ProviderRequest& request) {
        if (!request.isValid()) {
            return buildErrorResponse("Invalid provider request.");
        }

        if (request.provider() != icodental::domain::ProviderType::Ollama) {
            return buildErrorResponse(
                "OllamaClient received a non-Ollama request.");
        }

        if (m_baseUrl.trimmed().isEmpty()) {
            return buildErrorResponse("Ollama base URL is empty.");
        }

        if (m_model.trimmed().isEmpty() && request.model().trimmed().isEmpty()) {
            return buildErrorResponse("Ollama model is empty.");
        }

        if (m_networkExecutor == nullptr) {
            return buildErrorResponse(
                "Ollama network executor is not configured.");
        }

        return buildErrorResponse("Ollama analysis is not implemented yet.");
    }

    ProviderResponse OllamaClient::buildErrorResponse(const QString& message) const {
        return ProviderResponse(false, QString(), QString(), message);
    }

    QUrl OllamaClient::buildEndpoint() const {
        QUrl url(m_baseUrl.trimmed());

        QString path = url.path();
        if (!path.endsWith('/')) {
            path.append('/');
        }
        path.append("api/chat");

        url.setPath(path);
        return url;
    }
}