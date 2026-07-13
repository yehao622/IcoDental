#pragma once

#include <QString>

#include "domain/ProviderType.hpp"

namespace icodental::infrastructure::config {
	class AppConfig {
		public:
		    [[nodiscard]] const QString& defaultWorkingDirectory() const;
		    void setDefaultWorkingDirectory(QString value);

		    [[nodiscard]] icodental::domain::ProviderType defaultProvider() const;
		    void setDefaultProvider(icodental::domain::ProviderType value);

		    [[nodiscard]] const QString& ollamaBaseUrl() const;
		    void setOllamaBaseUrl(QString value);

		    [[nodiscard]] const QString& ollamaModel() const;
		    void setOllamaModel(QString value);

		    [[nodiscard]] const QString& geminiModel() const;
		    void setGeminiModel(QString value);

		    [[nodiscard]] bool geminiEnabled() const;
		    void setGeminiEnabled(bool value);

		    [[nodiscard]] bool analyzeNewOnly() const;
		    void setAnalyzeNewOnly(bool value);

		    [[nodiscard]] bool isValid() const;

		    static AppConfig createDefault();

		private:
			QString m_defaultWorkingDirectory;
		    icodental::domain::ProviderType m_defaultProvider = icodental::domain::ProviderType::Ollama;
		    QString m_ollamaBaseUrl;
		    QString m_ollamaModel;
		    QString m_geminiModel;
		    bool m_geminiEnabled = false;
		    bool m_analyzeNewOnly = true;
	};
}
