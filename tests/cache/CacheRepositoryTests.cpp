#include <QDateTime>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>
#include <QSqlDatabase>

#include "domain/ImageFingerprint.hpp"
#include "domain/ProviderType.hpp"
#include "infrastructure/cache/AnalysisCacheEntry.hpp"
#include "infrastructure/cache/AnalysisCacheRepository.hpp"
#include "infrastructure/cache/CacheDatabase.hpp"

namespace cache = icodental::infrastructure::cache;
namespace domain = icodental::domain;

class CacheRepositoryTests : public QObject {
    Q_OBJECT

    private slots:
        void cacheDatabase_opensAndInitializesSchema();
        void repository_findByFingerprint_returnsEmptyWhenMissing();
        void repository_saveAndFindByFingerprint_roundTripsEntry();
        void repository_save_updatesExistingFingerprint();

    private:
        static QString makeDbPath(QTemporaryDir& tempDir, const QString& fileName);
        static cache::AnalysisCacheEntry makeEntry(
            const QString& fingerprint,
            domain::ProviderType provider,
            const QString& model,
            const QString& summary,
            const QDateTime& createdAt,
            const QDateTime& updatedAt);

    static void cleanupConnection(const QString& connectionName);
};

QString CacheRepositoryTests::makeDbPath(QTemporaryDir& tempDir, const QString& fileName) {
    return tempDir.filePath(fileName);
}

cache::AnalysisCacheEntry CacheRepositoryTests::makeEntry(
    const QString& fingerprint,
    domain::ProviderType provider,
    const QString& model,
    const QString& summary,
    const QDateTime& createdAt,
    const QDateTime& updatedAt) {
    return cache::AnalysisCacheEntry(
        domain::ImageFingerprint(fingerprint),
        provider,
        model,
        summary,
        createdAt,
        updatedAt);
}

void CacheRepositoryTests::cleanupConnection(const QString& connectionName) {
    if (connectionName.isEmpty()) {
        return;
    }

    {
        QSqlDatabase db = QSqlDatabase::database(connectionName, false);
        if (db.isValid() && db.isOpen()) {
            db.close();
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
}

void CacheRepositoryTests::cacheDatabase_opensAndInitializesSchema() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString dbPath = makeDbPath(tempDir, "cache_init.sqlite");

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());
        QVERIFY(database.lastError().isEmpty());

        QSqlDatabase db = database.database();
        QVERIFY(db.isValid());
        QVERIFY(db.isOpen());
        connectionName = db.connectionName();
        QVERIFY(!connectionName.isEmpty());
    }

    cleanupConnection(connectionName);
}

void CacheRepositoryTests::repository_findByFingerprint_returnsEmptyWhenMissing() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString dbPath = makeDbPath(tempDir, "cache_missing.sqlite");

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);
        const auto result =
            repository.findByFingerprint(domain::ImageFingerprint(QString(64, 'a')));

        QVERIFY(!result.has_value());
    }

    cleanupConnection(connectionName);
}

void CacheRepositoryTests::repository_saveAndFindByFingerprint_roundTripsEntry() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString dbPath = makeDbPath(tempDir, "cache_roundtrip.sqlite");
    const QDateTime now = QDateTime::currentDateTimeUtc();

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);
        const auto entry = makeEntry(
            QString(64, 'b'),
            domain::ProviderType::Gemini,
            "gemini-1.5-pro",
            "cached analysis result",
            now,
            now);

        QVERIFY(entry.isValid());
        QVERIFY(repository.save(entry));

        const auto found = repository.findByFingerprint(entry.fingerprint());
        QVERIFY(found.has_value());
        QVERIFY(found->isValid());
        QCOMPARE(found->fingerprint().sha256(), entry.fingerprint().sha256());
        QVERIFY(found->provider() == domain::ProviderType::Gemini);
        QCOMPARE(found->model(), QString("gemini-1.5-pro"));
        QCOMPARE(found->summaryText(), QString("cached analysis result"));
    }

    cleanupConnection(connectionName);
}

void CacheRepositoryTests::repository_save_updatesExistingFingerprint() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString dbPath = makeDbPath(tempDir, "cache_update.sqlite");
    const QDateTime createdAt = QDateTime::currentDateTimeUtc();
    const QDateTime updatedAt = createdAt.addSecs(60);
    const QString fingerprint = QString(64, 'c');

    QString connectionName;
    {
        cache::CacheDatabase database(dbPath);
        QVERIFY(database.open());
        QVERIFY(database.initializeSchema());

        QSqlDatabase db = database.database();
        connectionName = db.connectionName();

        cache::AnalysisCacheRepository repository(db);

        const auto first = makeEntry(
            fingerprint,
            domain::ProviderType::Gemini,
            "gemini-2.5-flash",
            "first summary",
            createdAt,
            createdAt);

        const auto second = makeEntry(
            fingerprint,
            domain::ProviderType::Ollama,
            "llava",
            "updated summary",
            createdAt,
            updatedAt);

        QVERIFY(repository.save(first));
        QVERIFY(repository.save(second));

        const auto found = repository.findByFingerprint(domain::ImageFingerprint(fingerprint));
        QVERIFY(found.has_value());
        QVERIFY(found->provider() == domain::ProviderType::Ollama);
        QCOMPARE(found->model(), QString("llava"));
        QCOMPARE(found->summaryText(), QString("updated summary"));
        QCOMPARE(found->createdAtUtc().toUTC().toString(Qt::ISODate),
                 createdAt.toUTC().toString(Qt::ISODate));
        QCOMPARE(found->updatedAtUtc().toUTC().toString(Qt::ISODate),
                 updatedAt.toUTC().toString(Qt::ISODate));
    }

    cleanupConnection(connectionName);
}

QTEST_APPLESS_MAIN(CacheRepositoryTests)

#include "CacheRepositoryTests.moc"