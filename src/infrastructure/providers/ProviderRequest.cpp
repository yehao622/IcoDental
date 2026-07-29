#include "infrastructure/providers/ProviderRequest.hpp"

#include <utility>

namespace icodental::infrastructure::providers {
    ProviderRequest::ProviderRequest(
        icodental::domain::ProviderType provider,
        QString model,
        QString prompt,
        QString imagePath)
        : m_provider(provider)
        , m_model(std::move(model))
        , m_prompt(std::move(prompt))
        , m_imagePath(std::move(imagePath))
    {}

    bool ProviderRequest::isValid() const {
        return m_provider != icodental::domain::ProviderType::Unknown
            && !m_model.trimmed().isEmpty()
            && !m_prompt.trimmed().isEmpty()
            && !m_imagePath.trimmed().isEmpty();
    }

    icodental::domain::ProviderType ProviderRequest::provider() const {
        return m_provider;
    }

    const QString& ProviderRequest::model() const {
        return m_model;
    }

    const QString& ProviderRequest::prompt() const {
        return m_prompt;
    }

    const QString& ProviderRequest::imagePath() const {
        return m_imagePath;
    }
}