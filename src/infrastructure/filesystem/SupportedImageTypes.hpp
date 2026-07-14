#pragma once

#include <QString>
#include <QStringList>

namespace icodental::infrastructure::filesystem {

class SupportedImageTypes {
    public:
        static QStringList extensions();
        static bool isSupportedFile(const QString& filePath);
    };
}