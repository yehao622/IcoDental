#include "ImageFingerprint.hpp"

namespace icodental::domain {

    ImageFingerprint::ImageFingerprint(QString sha256)
        : m_sha256(std::move(sha256))
    {}

    bool ImageFingerprint::isValid() const {
        return !m_sha256.trimmed().isEmpty();
    }

    const QString& ImageFingerprint::sha256() const {
        return m_sha256;
    }

    QJsonObject ImageFingerprint::toJson() const {
        return QJsonObject{
            {"sha256", m_sha256}
        };
    }

    ImageFingerprint ImageFingerprint::fromJson(const QJsonObject& json) {
        return ImageFingerprint(json.value("sha256").toString());
    }

}