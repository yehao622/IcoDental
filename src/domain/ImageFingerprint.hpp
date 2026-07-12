#pragma once

#include <QJsonObject>
#include <QString>

namespace icodental::domain {

    class ImageFingerprint {
        public:
            ImageFingerprint() = default;
            explicit ImageFingerprint(QString sha256);

            [[nodiscard]] bool isValid() const;
            [[nodiscard]] const QString& sha256() const;

            [[nodiscard]] QJsonObject toJson() const;
            static ImageFingerprint fromJson(const QJsonObject& json);

            friend bool operator==(const ImageFingerprint& lhs, const ImageFingerprint& rhs) = default;

        private:
            QString m_sha256;
    };
}