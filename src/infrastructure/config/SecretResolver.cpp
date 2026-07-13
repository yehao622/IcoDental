#include "SecretResolver.hpp"

#include <QProcessEnvironment>

namespace icodental::infrastructure::config {
    QString SecretResolver::geminiApiKey() {
        const auto env = QProcessEnvironment::systemEnvironment();

        if (env.contains("GEMINI_API_KEY")) {
            return env.value("GEMINI_API_KEY").trimmed();
        }

        if (env.contains("GOOGLE_API_KEY")) {
            return env.value("GOOGLE_API_KEY").trimmed();
        }

        return {};
    }
}