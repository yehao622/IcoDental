#include "infrastructure/providers/GeminiClient.hpp"

#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "domain/ProviderType.hpp"
#include "infrastructure/providers/GeminiPayloadBuilder.hpp"
#include "infrastructure/providers/GeminiResponseParser.hpp"

namespace icodental::infrastructure::providers {
    GeminiClient::GeminiClient(QString apiKey, QNetworkAccessManager* networkAccessManager)
        : m_apiKey(std::move(apiKey))
        , m_networkAccessManager(networkAccessManager)
        , m_ownsNetworkAccessManager(networkAccessManager == nullptr)
    {
        if (m_ownsNetworkAccessManager) {
            m_networkAccessManager = new QNetworkAccessManager();
        }
    }

    GeminiClient::~GeminiClient() {
        if (m_ownsNetworkAccessManager) {
            delete m_networkAccessManager;
            m_networkAccessManager = nullptr;
        }
    }

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

        QFile imageFile(request.imagePath());
        if (!imageFile.open(QIODevice::ReadOnly)) {
            return buildErrorResponse(QString("Failed to open image file: %1").arg(request.imagePath()));
        }

        const QByteArray imageBytes = imageFile.readAll();

        GeminiPayloadBuilder payloadBuilder;
        const QJsonObject payload = payloadBuilder.build(request, imageBytes);

        const QString endpoint =
            QString("https://generativelanguage.googleapis.com/v1beta/models/%1:generateContent?key=%2")
                .arg(request.model(), m_apiKey);

        QNetworkRequest networkRequest{QUrl(endpoint)};
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* reply =
            m_networkAccessManager->post(networkRequest, QJsonDocument(payload).toJson());

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        const QByteArray responseBytes = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            const QString errorMessage = reply->errorString();
            const QString rawResponse = QString::fromUtf8(responseBytes);
            reply->deleteLater();
            return ProviderResponse(false, QString(), rawResponse, errorMessage);
        }

        reply->deleteLater();

        GeminiResponseParser responseParser;
        return responseParser.parse(responseBytes);
    }

    ProviderResponse GeminiClient::buildErrorResponse(const QString& message) const {
        return ProviderResponse(false, QString(), QString(), message);
    }
}