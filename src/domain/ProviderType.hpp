#pragma once

#include <QString>
#include <optional>

namespace icodental::domain {
	enum class ProviderType {
	    Ollama,
	    Gemini,
	    Unknown
	};

	QString toString(ProviderType provider);
	std::optional<ProviderType> providerTypeFromString(const QString& value);
}