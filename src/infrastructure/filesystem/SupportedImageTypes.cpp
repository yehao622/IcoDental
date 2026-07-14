#include "infrastructure/filesystem/SupportedImageTypes.hpp"

#include <QFileInfo>

namespace icodental::infrastructure::filesystem {
    QStringList SupportedImageTypes::extensions() {
        return {
            "jpg",
            "jpeg",
            "png",
            "bmp",
            "webp",
            "tif",
            "tiff"
        };
    }

    bool SupportedImageTypes::isSupportedFile(const QString& filePath) {
        const QFileInfo info(filePath);

        if (!info.exists() || !info.isFile()) {
            return false;
        }

        return extensions().contains(info.suffix().toLower());
    }
}