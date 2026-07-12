#include "ProviderType.hpp"

namespace icodental::domain {
	QString toString(ProviderType provider) {
		switch (provider) {
		    case ProviderType::Ollama: return "ollama";
		    case ProviderType::Gemini: return "gemini";
		    case ProviderType::Unknown: return "unknown";
	    }
	    return "unknown";
	}
	
	std::optional<ProviderType> providerTypeFromString(const QString& value){
	    if (value == "ollama") return ProviderType::Ollama;
	    if (value == "gemini") return ProviderType::Gemini;
	    if (value == "unknown") return ProviderType::Unknown;
	    return std::nullopt;
	}
}