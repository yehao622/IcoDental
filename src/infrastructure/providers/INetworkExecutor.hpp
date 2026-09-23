#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <QUrl>

namespace icodental::infrastructure::providers {
	struct NetworkResult {
	    bool success{false};
	    QByteArray responseBody;
	    QString errorMessage;
	};

	class INetworkExecutor {
		public:
		    virtual ~INetworkExecutor() = default;

		    [[nodiscard]] virtual NetworkResult postJson(
		        const QUrl& url,
		        const QJsonObject& payload,
    			int timeoutMilliseconds = -1) = 0;
	};
}