#include "infrastructure/cache/CacheDatabase.hpp"

#include <QUuid>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <utility>

namespace icodental::infrastructure::cache {
    CacheDatabase::CacheDatabase(QString databasePath)
        : m_databasePath(std::move(databasePath))
        , m_connectionName(QUuid::createUuid().toString(QUuid::WithoutBraces))
    {}

    bool CacheDatabase::open() {
        auto db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        db.setDatabaseName(m_databasePath);

        if (!db.open()) {
            m_lastError = db.lastError().text();
            return false;
        }

        return true;
    }

    bool CacheDatabase::initializeSchema() {
        auto db = database();
        if (!db.isValid() || !db.isOpen()) {
            m_lastError = "Database connection is not open";
            return false;
        }

        QSqlQuery query(db);
        const bool ok = query.exec(R"(
            CREATE TABLE IF NOT EXISTS analysis_cache (
                fingerprint TEXT PRIMARY KEY,
                provider TEXT NOT NULL,
                model TEXT NOT NULL,
                summary_text TEXT NOT NULL,
                created_at_utc TEXT NOT NULL,
                updated_at_utc TEXT NOT NULL
            )
        )");

        if (!ok) {
            m_lastError = query.lastError().text();
            return false;
        }

        return true;
    }

    QString CacheDatabase::lastError() const {
        return m_lastError;
    }

    QSqlDatabase CacheDatabase::database() const {
        return QSqlDatabase::database(m_connectionName);
    }
}