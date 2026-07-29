#include "infrastructure/providers/QtNetworkExecutor.hpp"

#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace icodental::infrastructure::providers {
    QtNetworkExecutor::QtNetworkExecutor()
        : m_networkAccessManager(new QNetworkAccessManager())
        , m_ownsNetworkAccessManager(true)
    {}

    QtNetworkExecutor::QtNetworkExecutor(QNetworkAccessManager* networkAccessManager)
        : m_networkAccessManager(networkAccessManager)
        , m_ownsNetworkAccessManager(false) {
        if (!m_networkAccessManager) {
            m_networkAccessManager = new QNetworkAccessManager();
            m_ownsNetworkAccessManager = true;
        }
    }

    QtNetworkExecutor::~QtNetworkExecutor() {
        if (m_ownsNetworkAccessManager) {
            delete m_networkAccessManager;
            m_networkAccessManager = nullptr;
        }
    }

    NetworkResult QtNetworkExecutor::postJson(
        const QUrl& url,
        const QJsonObject& payload) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* reply =
            m_networkAccessManager->post(request, QJsonDocument(payload).toJson());

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        NetworkResult result;
        result.responseBody = reply->readAll();

        if (reply->error() == QNetworkReply::NoError) {
            result.success = true;
        } else {
            result.success = false;
            result.errorMessage = reply->errorString();
        }

        reply->deleteLater();
        return result;
    }
}