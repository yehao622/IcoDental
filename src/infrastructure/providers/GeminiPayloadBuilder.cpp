#include "infrastructure/providers/GeminiPayloadBuilder.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace icodental::infrastructure::providers {
    QJsonObject GeminiPayloadBuilder::build(
        const ProviderRequest& request,
        const QByteArray& imageBytes) const
    {
        const QByteArray imageBase64 = imageBytes.toBase64();

        QJsonObject inlineData{
            {"mimeType", "image/jpeg"},
            {"data", QString::fromLatin1(imageBase64)}
        };

        QJsonArray parts;
        parts.append(QJsonObject{{"text", request.prompt()}});
        parts.append(QJsonObject{{"inlineData", inlineData}});

        return QJsonObject{
            {"contents", QJsonArray{
                QJsonObject{
                    {"parts", parts}
                }
            }}
        };
    }
}