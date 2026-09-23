#include "infrastructure/providers/QtNetworkExecutor.hpp"

#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

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
        const QJsonObject& payload,
        int timeoutMilliseconds) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        QNetworkReply* reply =
            m_networkAccessManager->post(request, QJsonDocument(payload).toJson());

        QEventLoop loop;
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);

        bool timedOut = false;

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        QObject::connect(
            &timeoutTimer,
            &QTimer::timeout,
            &loop,
            [&reply, &loop, &timedOut] {
                timedOut = true;

                if (reply->isRunning()) {
                    reply->abort();
                }

                loop.quit();
            });

        if (timeoutMilliseconds > 0) {
            timeoutTimer.start(timeoutMilliseconds);
        }

        loop.exec();
        if (timeoutTimer.isActive()) {
            timeoutTimer.stop();
        }

        NetworkResult result;
        result.responseBody = reply->readAll();

        if (timedOut) {
            result.success = false;
            result.errorMessage = QString(
                "Provider request timed out after %1 seconds.")
                .arg(timeoutMilliseconds / 1000);

            reply->deleteLater();
            return result;
        }

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