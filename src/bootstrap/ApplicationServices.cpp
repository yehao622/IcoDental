#include "bootstrap/ApplicationServices.hpp"

#include <QDir>
#include <QStandardPaths>

#include <memory>
#include <utility>

#include "infrastructure/filesystem/FileScanner.hpp"
#include "infrastructure/filesystem/ImageHasher.hpp"

namespace icodental::bootstrap {
    ApplicationServices::ApplicationServices(QString cacheDatabasePath)
        : m_cacheDatabase(std::move(cacheDatabasePath))
    {}

    bool ApplicationServices::initialize() {
        if (isInitialized()) {
            return true;
        }

        if (!m_cacheDatabase.open()) {
            m_lastError = m_cacheDatabase.lastError();
            return false;
        }

        if (!m_cacheDatabase.initializeSchema()) {
            m_lastError = m_cacheDatabase.lastError();
            return false;
        }

        m_cacheRepository =
            std::make_unique<infrastructure::cache::AnalysisCacheRepository>(
                m_cacheDatabase.database());

        m_analysisOrchestrator =
            std::make_unique<application::AnalysisOrchestrator>(
                infrastructure::filesystem::FileScanner{},
                infrastructure::filesystem::ImageHasher{},
                *m_cacheRepository);

        return true;
    }

    bool ApplicationServices::isInitialized() const {
        return m_cacheRepository != nullptr
            && m_analysisOrchestrator != nullptr;
    }

    const QString& ApplicationServices::lastError() const {
        return m_lastError;
    }

    application::AnalysisOrchestrator& ApplicationServices::analysisOrchestrator() {
        return *m_analysisOrchestrator;
    }

    infrastructure::cache::AnalysisCacheRepository& ApplicationServices::cacheRepository() {
        return *m_cacheRepository;
    }

    QString ApplicationServices::defaultCacheDatabasePath() {
        QString dataDirectory = QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

        if (dataDirectory.isEmpty()) {
            dataDirectory = QDir::homePath() + "/.local/share/IcoDental";
        }

        QDir().mkpath(dataDirectory);

        return QDir(dataDirectory).filePath("icodental.sqlite");
    }
}