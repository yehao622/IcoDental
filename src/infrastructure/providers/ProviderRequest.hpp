#pragma once

#include <QString>

#include "domain/ProviderType.hpp"

namespace icodental::infrastructure::providers {
    class ProviderRequest {
        public:
            ProviderRequest(
                icodental::domain::ProviderType provider,
                QString model,
                QString prompt,
                QString imagePath);

            [[nodiscard]] bool isValid() const;

            [[nodiscard]] icodental::domain::ProviderType provider() const;
            [[nodiscard]] const QString& model() const;
            [[nodiscard]] const QString& prompt() const;
            [[nodiscard]] const QString& imagePath() const;

        private:
            icodental::domain::ProviderType m_provider{icodental::domain::ProviderType::Unknown};
            QString m_model;
            QString m_prompt;
            QString m_imagePath;
    };
}