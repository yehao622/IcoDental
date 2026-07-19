#include "infrastructure/providers/ProviderResponse.hpp"

#include <utility>

namespace icodental::infrastructure::providers {
    ProviderResponse::ProviderResponse(
        bool success,
        QString summaryText,
        QString rawResponse,
        QString errorMessage)
        : m_success(success)
        , m_summaryText(std::move(summaryText))
        , m_rawResponse(std::move(rawResponse))
        , m_errorMessage(std::move(errorMessage))
    {}

    bool ProviderResponse::success() const {
        return m_success;
    }

    const QString& ProviderResponse::summaryText() const {
        return m_summaryText;
    }

    const QString& ProviderResponse::rawResponse() const {
        return m_rawResponse;
    }

    const QString& ProviderResponse::errorMessage() const {
        return m_errorMessage;
    }

    bool ProviderResponse::isValid() const {
        return m_success && !m_summaryText.trimmed().isEmpty();
    }
}