#include "infrastructure/providers/GeminiClient.hpp"

#include <QFile>
#include <QJsonObject>
#include <QUrl>
#include <utility>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/GeminiPayloadBuilder.hpp"
#include "infrastructure/providers/GeminiResponseParser.hpp"
#include "infrastructure/providers/QtNetworkExecutor.hpp"

namespace icodental::infrastructure::providers {
    GeminiClient::GeminiClient(QString apiKey)
        : m_apiKey(std::move(apiKey))
        , m_ownedNetworkExecutor(std::make_unique<QtNetworkExecutor>())
        , m_networkExecutor(m_ownedNetworkExecutor.get())
    {}

    GeminiClient::GeminiClient(QString apiKey, INetworkExecutor* networkExecutor)
        : m_apiKey(std::move(apiKey))
        , m_networkExecutor(networkExecutor)
    {}

    ProviderResponse GeminiClient::analyze(const ProviderRequest& request) {
        if (!request.isValid()) {
            return buildErrorResponse("Invalid provider request.");
        }

        if (request.provider() != icodental::domain::ProviderType::Gemini) {
            return buildErrorResponse("GeminiClient received a non-Gemini request.");
        }

        if (m_apiKey.trimmed().isEmpty()) {
            return buildErrorResponse("Gemini API key is empty.");
        }

        if (m_networkExecutor == nullptr) {
            return buildErrorResponse("Gemini network executor is not configured.");
        }

        QFile imageFile(request.imagePath());
        if (!imageFile.open(QIODevice::ReadOnly)) {
            return buildErrorResponse(QString("Failed to open image file: %1").arg(request.imagePath()));
        }

        const QByteArray imageBytes = imageFile.readAll();

        GeminiPayloadBuilder payloadBuilder;
        const QJsonObject payload = payloadBuilder.build(request, imageBytes);

        const NetworkResult result = m_networkExecutor->postJson(QUrl(buildEndpoint(request.model())), payload);

        if (!result.success) {
            return ProviderResponse(false, QString(), QString::fromUtf8(result.responseBody), result.errorMessage);
        }

        GeminiResponseParser parser;
        return parser.parse(result.responseBody);
    }

    ProviderResponse GeminiClient::buildErrorResponse(const QString& message) const {
        return ProviderResponse(false, QString(), QString(), message);
    }

    QString GeminiClient::buildEndpoint(const QString& model) const {
        return QString(
            "https://generativelanguage.googleapis.com/v1beta/models/%1:generateContent?key=%2")
            .arg(model, m_apiKey);
    }
}