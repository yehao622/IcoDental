#pragma once

#include <QString>

#include "domain/ImageFingerprint.hpp"

namespace icodental::infrastructure::filesystem {

class ImageHasher {
    public:
        [[nodiscard]] icodental::domain::ImageFingerprint hashFile(const QString& filePath) const;
    };
}