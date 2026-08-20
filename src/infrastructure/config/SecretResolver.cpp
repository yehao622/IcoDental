#include "SecretResolver.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QTextStream>

namespace icodental::infrastructure::config {
    QString findDotEnvPath() {
        QDir directory(QCoreApplication::applicationDirPath());

        for (int level = 0; level < 5; ++level) {
            const QString candidate = directory.filePath(".env");

            if (QFile::exists(candidate)) {
                return candidate;
            }

            if (!directory.cdUp()) {
                break;
            }
        }

        return {};
    }

    QString readDotEnvValue(const QString& key) {
        const QString dotEnvPath = findDotEnvPath();

        if (dotEnvPath.isEmpty()) {
            return {};
        }

        QFile file(QDir::cleanPath(dotEnvPath));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return {};
        }

        QTextStream stream(&file);

        while (!stream.atEnd()) {
            const QString line = stream.readLine().trimmed();

            if (line.isEmpty() || line.startsWith('#')) {
                continue;
            }

            const int equalsIndex = line.indexOf('=');
            if (equalsIndex <= 0) {
                continue;
            }

            const QString entryKey = line.left(equalsIndex).trimmed();
            if (entryKey != key) {
                continue;
            }

            QString value = line.mid(equalsIndex + 1).trimmed();

            if (value.size() >= 2
                && ((value.startsWith('"') && value.endsWith('"'))
                    || (value.startsWith('\'') && value.endsWith('\'')))) {
                value = value.mid(1, value.size() - 2);
            }

            return value;
        }

        return {};
    }

    QString SecretResolver::geminiApiKey() {
        const QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

        const QString geminiApiKey =
            environment.value("GEMINI_API_KEY").trimmed();

        if (!geminiApiKey.isEmpty()) {
            return geminiApiKey;
        }

        const QString googleApiKey =
            environment.value("GOOGLE_API_KEY").trimmed();

        if (!googleApiKey.isEmpty()) {
            return googleApiKey;
        }

        return readDotEnvValue("GEMINI_API_KEY").trimmed();
    }
}