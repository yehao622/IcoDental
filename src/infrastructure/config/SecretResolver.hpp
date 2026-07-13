#pragma once

#include <QString>

namespace icodental::infrastructure::config {
    class SecretResolver {
        public:
            static QString geminiApiKey();
    };
}