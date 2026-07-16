#include "application/AnalysisRequest.hpp"

#include <utility>

namespace icodental::application {
    AnalysisRequest::AnalysisRequest(
        InputMode inputMode,
        QString inputPath,
        QStringList explicitFiles,
        domain::ProviderType provider,
        QString model,
        bool forceRefresh)
        : m_inputMode(inputMode)
        , m_inputPath(std::move(inputPath))
        , m_explicitFiles(std::move(explicitFiles))
        , m_provider(provider)
        , m_model(std::move(model))
        , m_forceRefresh(forceRefresh)
    {}

    AnalysisRequest::InputMode AnalysisRequest::inputMode() const {
        return m_inputMode;
    }

    const QString& AnalysisRequest::inputPath() const {
        return m_inputPath;
    }

    const QStringList& AnalysisRequest::explicitFiles() const {
        return m_explicitFiles;
    }

    domain::ProviderType AnalysisRequest::provider() const {
        return m_provider;
    }

    const QString& AnalysisRequest::model() const {
        return m_model;
    }

    bool AnalysisRequest::forceRefresh() const {
        return m_forceRefresh;
    }

    bool AnalysisRequest::isValid() const {
        if (m_provider == domain::ProviderType::Unknown) {
            return false;
        }

        if (m_model.trimmed().isEmpty()) {
            return false;
        }

        switch (m_inputMode) {
        case InputMode::Directory:
            return !m_inputPath.trimmed().isEmpty();
        case InputMode::ExplicitFiles:
            return !m_explicitFiles.isEmpty();
        }

        return false;
    }
}