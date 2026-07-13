#pragma once

#include <QString>
#include <QStringList>

#include "domain/ProviderType.hpp"

namespace icodental::infrastructure::config {
	class ModelCatalog {
	public:
	    static QString defaultModelFor(icodental::domain::ProviderType provider);
	    static QStringList availableModelsFor(icodental::domain::ProviderType provider);
	    static bool isSupportedModel(icodental::domain::ProviderType provider, const QString& model);
	};
}