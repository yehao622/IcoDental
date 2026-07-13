#include "SettingsRepository.hpp"

#include <QSettings>

#include "domain/ProviderType.hpp"

namespace icodental::infrastructure::config {
    using icodental::domain::ProviderType;
    using icodental::domain::providerTypeFromString;
    using icodental::domain::toString;

    namespace {
        constexpr auto kWorkingDirectoryKey = "general/defaultWorkingDirectory";
        constexpr auto kDefaultProviderKey = "general/defaultProvider";
        constexpr auto kAnalyzeNewOnlyKey = "general/analyzeNewOnly";

        constexpr auto kOllamaBaseUrlKey = "ollama/baseUrl";
        constexpr auto kOllamaModelKey = "ollama/model";

        constexpr auto kGeminiEnabledKey = "gemini/enabled";
        constexpr auto kGeminiModelKey = "gemini/model";
    }

    AppConfig SettingsRepository::load() const {
        AppConfig config = AppConfig::createDefault();
        QSettings settings(kOrganization, kApplication);

        config.setDefaultWorkingDirectory(
            settings.value(kWorkingDirectoryKey, config.defaultWorkingDirectory()).toString());

        const auto providerString =
            settings.value(kDefaultProviderKey, toString(config.defaultProvider())).toString();
        config.setDefaultProvider(
            providerTypeFromString(providerString).value_or(ProviderType::Ollama));

        config.setAnalyzeNewOnly(
            settings.value(kAnalyzeNewOnlyKey, config.analyzeNewOnly()).toBool());

        config.setOllamaBaseUrl(
            settings.value(kOllamaBaseUrlKey, config.ollamaBaseUrl()).toString());

        config.setOllamaModel(
            settings.value(kOllamaModelKey, config.ollamaModel()).toString());

        config.setGeminiEnabled(
            settings.value(kGeminiEnabledKey, config.geminiEnabled()).toBool());

        config.setGeminiModel(
            settings.value(kGeminiModelKey, config.geminiModel()).toString());

        return config.isValid() ? config : AppConfig::createDefault();
    }

    void SettingsRepository::save(const AppConfig& config) const {
        QSettings settings(kOrganization, kApplication);

        settings.setValue(kWorkingDirectoryKey, config.defaultWorkingDirectory());
        settings.setValue(kDefaultProviderKey, toString(config.defaultProvider()));
        settings.setValue(kAnalyzeNewOnlyKey, config.analyzeNewOnly());

        settings.setValue(kOllamaBaseUrlKey, config.ollamaBaseUrl());
        settings.setValue(kOllamaModelKey, config.ollamaModel());

        settings.setValue(kGeminiEnabledKey, config.geminiEnabled());
        settings.setValue(kGeminiModelKey, config.geminiModel());

        settings.sync();
    }
}
