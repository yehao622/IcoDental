#include "AppConfig.hpp"

#include <QDir>

#include "ModelCatalog.hpp"

namespace icodental::infrastructure::config {
    using icodental::domain::ProviderType;

    const QString& AppConfig::defaultWorkingDirectory() const {
        return m_defaultWorkingDirectory;
    }

    void AppConfig::setDefaultWorkingDirectory(QString value) {
        m_defaultWorkingDirectory = std::move(value);
    }

    ProviderType AppConfig::defaultProvider() const {
        return m_defaultProvider;
    }

    void AppConfig::setDefaultProvider(ProviderType value) {
        m_defaultProvider = value;
    }

    const QString& AppConfig::ollamaBaseUrl() const {
        return m_ollamaBaseUrl;
    }

    void AppConfig::setOllamaBaseUrl(QString value) {
        m_ollamaBaseUrl = std::move(value);
    }

    const QString& AppConfig::ollamaModel() const {
        return m_ollamaModel;
    }

    void AppConfig::setOllamaModel(QString value) {
        m_ollamaModel = std::move(value);
    }

    const QString& AppConfig::geminiModel() const {
        return m_geminiModel;
    }

    void AppConfig::setGeminiModel(QString value) {
        m_geminiModel = std::move(value);
    }

    bool AppConfig::geminiEnabled() const {
        return m_geminiEnabled;
    }

    void AppConfig::setGeminiEnabled(bool value) {
        m_geminiEnabled = value;
    }

    bool AppConfig::analyzeNewOnly() const {
        return m_analyzeNewOnly;
    }

    void AppConfig::setAnalyzeNewOnly(bool value) {
        m_analyzeNewOnly = value;
    }

    bool AppConfig::isValid() const {
        if (m_defaultWorkingDirectory.trimmed().isEmpty()) {
            return false;
        }

        if (m_ollamaBaseUrl.trimmed().isEmpty()) {
            return false;
        }

        if (!ModelCatalog::isSupportedModel(ProviderType::Ollama, m_ollamaModel)) {
            return false;
        }

        if (!m_geminiModel.isEmpty() &&
            !ModelCatalog::isSupportedModel(ProviderType::Gemini, m_geminiModel)) {
            return false;
        }

        return true;
    }

    AppConfig AppConfig::createDefault() {
        AppConfig config;
        config.setDefaultWorkingDirectory(QDir::homePath() + "/side_project/IcoDental/LabSheet");
        config.setDefaultProvider(ProviderType::Ollama);
        config.setOllamaBaseUrl("http://127.0.0.1:11434");
        config.setOllamaModel(ModelCatalog::defaultModelFor(ProviderType::Ollama));
        config.setGeminiModel(ModelCatalog::defaultModelFor(ProviderType::Gemini));
        config.setGeminiEnabled(false);
        config.setAnalyzeNewOnly(true);
        return config;
    }
}