#pragma once

#include <QString>
#include <QStringList>

namespace icodental::infrastructure::filesystem {

class FileScanner {
    public:
        [[nodiscard]] QStringList scanDirectory(const QString& directoryPath) const;
        [[nodiscard]] QStringList filterSupportedFiles(const QStringList& filePaths) const;

    private:
        [[nodiscard]] QStringList deduplicateAndSort(QStringList filePaths) const;
    };
}