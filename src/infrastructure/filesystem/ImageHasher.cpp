#include "infrastructure/filesystem/ImageHasher.hpp"

#include <QCryptographicHash>
#include <QFile>

namespace icodental::infrastructure::filesystem {
    using icodental::domain::ImageFingerprint;

    ImageFingerprint ImageHasher::hashFile(const QString& filePath) const {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return ImageFingerprint{};
        }

        QCryptographicHash hash(QCryptographicHash::Sha256);

        if (!hash.addData(&file)) {
            return ImageFingerprint{};
        }

        return ImageFingerprint(QString::fromLatin1(hash.result().toHex()));
    }
}