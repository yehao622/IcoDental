#include "infrastructure/providers/OllamaClient.hpp"

#include <utility>

#include <QUrl>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

        QFile imageFile(request.imagePath());
        if (!imageFile.open(QIODevice::ReadOnly)) {
            return buildErrorResponse(
                QString("Failed to open image file: %1").arg(request.imagePath()));
        }

        const QString selectedModel =
            request.model().trimmed().isEmpty()
                ? m_model
                : request.model().trimmed();

        const QString imageBase64 =
            QString::fromLatin1(imageFile.readAll().toBase64());

        const QJsonObject requestMessage{
            {"role", "user"},
            {"content", request.prompt()},
            {"images", QJsonArray{imageBase64}}
        };

        const QJsonObject payload{
            {"model", selectedModel},
            {"stream", false},
            {"format", "json"},
            {"messages", QJsonArray{requestMessage}}
        };

        const QUrl endpoint = buildEndpoint();

        const NetworkResult result =
            m_networkExecutor->postJson(endpoint, payload);

        if (!result.success) {
            return ProviderResponse(
                false,
                QString(),
                QString::fromUtf8(result.responseBody),
                result.errorMessage);
        }

        const QString rawResponse = QString::fromUtf8(result.responseBody);
        const QJsonDocument responseDocument = QJsonDocument::fromJson(result.responseBody);
        if (!responseDocument.isObject()) {
            return ProviderResponse(
                false,
                QString(),
                rawResponse,
                "Ollama response was not a JSON object.");
        }

        const QJsonObject responseMessage = responseDocument.object().value("message").toObject();
        const QString content = responseMessage.value("content").toString().trimmed();

        if (content.isEmpty()) {
            return ProviderResponse(
                false,
                QString(),
                rawResponse,
                "Ollama response contained no message content.");
        }

        return ProviderResponse(
            true,
            content,
            rawResponse,
            QString());
    }

    ProviderResponse OllamaClient::buildErrorResponse(const QString& message) const {
        return ProviderResponse(false, QString(), QString(), message);
    }

    QUrl OllamaClient::buildEndpoint() const {
        QUrl url(m_baseUrl.trimmed());

        url.setPath("/api/chat");
        url.setQuery(QString());

        return url;
    }
}