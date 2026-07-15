#pragma once

#include <QString>

class QSqlDatabase;

namespace icodental::infrastructure::cache {

class CacheDatabase {
    public:
        explicit CacheDatabase(QString databasePath);

        [[nodiscard]] bool open();
        [[nodiscard]] bool initializeSchema();
        [[nodiscard]] QString lastError() const;
        [[nodiscard]] QSqlDatabase database() const;

    private:
        QString m_databasePath;
        QString m_connectionName;
        QString m_lastError;
    };
}