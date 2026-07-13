#include "ModelCatalog.hpp"

namespace icodental::infrastructure::config {
	using icodental::domain::ProviderType;


	QString ModelCatalog::defaultModelFor(ProviderType provider) {
		switch (provider) {
		    case ProviderType::Ollama:
		        return "llama3.1:8b";
		    case ProviderType::Gemini:
		        return "gemini-2.5-flash";
		    case ProviderType::Unknown:
		        break;
	    }
	    return {};
	}

	QStringList ModelCatalog::availableModelsFor(ProviderType provider) {
		switch (provider) {
		    case ProviderType::Ollama:
		        return {
		            "llama3.1:8b",
		            "gemma4:e4b",
		            "deepseek-r1:8b",
		            "qwen2.5-coder:14b"
		        };
		    case ProviderType::Gemini:
		        return {
		            "gemini-2.5-flash"
		        };
		    case ProviderType::Unknown:
		        break;
	    }
	    return {};
	}

    bool ModelCatalog::isSupportedModel(ProviderType provider, const QString& model) {
    	return availableModelsFor(provider).contains(model);
	}
}