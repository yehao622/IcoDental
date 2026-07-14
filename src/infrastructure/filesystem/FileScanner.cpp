#include "infrastructure/filesystem/FileScanner.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <algorithm>

#include "infrastructure/filesystem/SupportedImageTypes.hpp"

namespace icodental::infrastructure::filesystem {
    QStringList FileScanner::scanDirectory(const QString& directoryPath) const {
        QStringList results;

        const QDir root(directoryPath);
        if (!root.exists()) {
            return results;
        }

        QDirIterator iterator(
            directoryPath,
            QDir::Files | QDir::NoDotAndDotDot,
            QDirIterator::Subdirectories);

        while (iterator.hasNext()) {
            const auto filePath = iterator.next();
            if (SupportedImageTypes::isSupportedFile(filePath)) {
                results.append(QFileInfo(filePath).canonicalFilePath());
            }
        }

        return deduplicateAndSort(std::move(results));
    }

    QStringList FileScanner::filterSupportedFiles(const QStringList& filePaths) const {
        QStringList results;

        for (const auto& path : filePaths) {
            if (SupportedImageTypes::isSupportedFile(path)) {
                results.append(QFileInfo(path).canonicalFilePath());
            }
        }

        return deduplicateAndSort(std::move(results));
    }

    QStringList FileScanner::deduplicateAndSort(QStringList filePaths) const {
        filePaths.removeDuplicates();

        std::sort(filePaths.begin(), filePaths.end(),  
                  [](const QString& left, const QString& right) {
                    return QString::compare(left, right, Qt::CaseInsensitive) < 0;
                  });

        return filePaths;
    }
}