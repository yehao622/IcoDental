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
        if (!query.exec(R"(
            CREATE TABLE IF NOT EXISTS analysis_cache (
                fingerprint TEXT PRIMARY KEY,
                provider TEXT NOT NULL,
                model TEXT NOT NULL,
                summary_text TEXT NOT NULL,
                case_analysis_result_json TEXT NOT NULL,
                created_at_utc TEXT NOT NULL,
                updated_at_utc TEXT NOT NULL
            )
        )")) {
            m_lastError = query.lastError().text();
            return false;
        }

        if (!query.exec(R"(
            PRAGMA table_info(analysis_cache)
        )")) {
            m_lastError = query.lastError().text();
            return false;
        }

        bool hasCaseAnalysisResultJson = false;
        while (query.next()) {
            if (query.value(1).toString() == "case_analysis_result_json") {
                hasCaseAnalysisResultJson = true;
                break;
            }
        }

        if (!hasCaseAnalysisResultJson) {
            if (!query.exec(R"(
                ALTER TABLE analysis_cache
                ADD COLUMN case_analysis_result_json TEXT NOT NULL DEFAULT '{}'
            )")) {
                m_lastError = query.lastError().text();
                return false;
            }
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