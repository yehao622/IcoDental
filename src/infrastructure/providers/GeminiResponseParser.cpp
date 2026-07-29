#include "infrastructure/providers/GeminiResponseParser.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace icodental::infrastructure::providers {
    ProviderResponse GeminiResponseParser::parse(const QByteArray& responseBytes) const {
        const QString rawResponse = QString::fromUtf8(responseBytes);

        const QJsonDocument jsonDocument = QJsonDocument::fromJson(responseBytes);
        if (!jsonDocument.isObject()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response was not a JSON object.");
        }

        const QJsonObject root = jsonDocument.object();
        const QJsonArray candidates = root.value("candidates").toArray();
        if (candidates.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response contained no candidates.");
        }

        const QJsonObject contentObject = candidates.first().toObject().value("content").toObject();
        const QJsonArray responseParts = contentObject.value("parts").toArray();
        if (responseParts.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response contained no text parts.");
        }

        const QString summaryText = responseParts.first().toObject().value("text").toString().trimmed();
        if (summaryText.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response text was empty.");
        }

        return ProviderResponse(true, summaryText, rawResponse, QString());
    }
}