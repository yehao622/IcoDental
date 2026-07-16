#include "application/AnalysisOrchestrator.hpp"

#include <utility>

namespace icodental::application {
    AnalysisOrchestrator::AnalysisOrchestrator(
        infrastructure::filesystem::FileScanner fileScanner,
        infrastructure::filesystem::ImageHasher imageHasher,
        infrastructure::cache::AnalysisCacheRepository cacheRepository)
        : m_fileScanner(std::move(fileScanner))
        , m_imageHasher(std::move(imageHasher))
        , m_cacheRepository(std::move(cacheRepository))
    {}

    AnalysisPlan AnalysisOrchestrator::buildPlan(const AnalysisRequest& request) const {
        QList<AnalysisItem> cachedItems;
        QList<AnalysisItem> pendingItems;
        QStringList errors;

        if (!request.isValid()) {
            errors.append("Invalid analysis request.");
            return AnalysisPlan(
                std::move(cachedItems),
                std::move(pendingItems),
                std::move(errors));
        }

        const QStringList filePaths = resolveInputFiles(request);
        if (filePaths.isEmpty()) {
            errors.append("No supported input files were found.");
            return AnalysisPlan(
                std::move(cachedItems),
                std::move(pendingItems),
                std::move(errors));
        }

        for (const auto& filePath : filePaths) {
            const auto fingerprint = m_imageHasher.hashFile(filePath);
            if (!fingerprint.isValid()) {
                errors.append(QString("Failed to hash file: %1").arg(filePath));
                continue;
            }

            if (!request.forceRefresh()) {
                const auto cachedEntry = m_cacheRepository.findByFingerprint(fingerprint);
                if (cachedEntry.has_value()) {
                    cachedItems.append(AnalysisItem(filePath, fingerprint, std::move(cachedEntry)));
                    continue;
                }
            }

            pendingItems.append(AnalysisItem(filePath, fingerprint, std::nullopt));
        }

        return AnalysisPlan(
            std::move(cachedItems),
            std::move(pendingItems),
            std::move(errors));
    }

    QStringList AnalysisOrchestrator::resolveInputFiles(const AnalysisRequest& request) const {
        switch (request.inputMode()) {
            case AnalysisRequest::InputMode::Directory:
                return m_fileScanner.scanDirectory(request.inputPath());

            case AnalysisRequest::InputMode::ExplicitFiles:
                return m_fileScanner.filterSupportedFiles(request.explicitFiles());
        }

        return {};
    }
}