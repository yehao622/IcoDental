#pragma once

#include <QString>

namespace icodental::infrastructure::providers {
    class ProviderResponse {
        public:
            ProviderResponse() = default;

            ProviderResponse(
                bool success,
                QString summaryText,
                QString rawResponse,
                QString errorMessage);

            [[nodiscard]] bool success() const;
            [[nodiscard]] const QString& summaryText() const;
            [[nodiscard]] const QString& rawResponse() const;
            [[nodiscard]] const QString& errorMessage() const;

            [[nodiscard]] bool isValid() const;

        private:
            bool m_success{false};
            QString m_summaryText;
            QString m_rawResponse;
            QString m_errorMessage;
    };
}