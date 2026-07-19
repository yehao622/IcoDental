#pragma once

#include <QNetworkAccessManager>

#include "infrastructure/providers/INetworkExecutor.hpp"

namespace icodental::infrastructure::providers {
    class QtNetworkExecutor : public INetworkExecutor {
        public:
            QtNetworkExecutor();
            explicit QtNetworkExecutor(QNetworkAccessManager* networkAccessManager);
            ~QtNetworkExecutor() override;

            [[nodiscard]] NetworkResult postJson(
                const QUrl& url,
                const QJsonObject& payload) override;

        private:
            QNetworkAccessManager* m_networkAccessManager{nullptr};
            bool m_ownsNetworkAccessManager{false};
    };
}