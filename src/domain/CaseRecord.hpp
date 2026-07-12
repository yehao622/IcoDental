#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

#include "AnalysisResult.hpp"
#include "CaseStatus.hpp"
#include "ImageFingerprint.hpp"

namespace icodental::domain {

class CaseRecord {
    public:
        [[nodiscard]] const ImageFingerprint& fingerprint() const;
        void setFingerprint(ImageFingerprint value);

        [[nodiscard]] const QString& fileName() const;
        void setFileName(QString value);

        [[nodiscard]] const QStringList& sourcePaths() const;
        void addSourcePath(const QString& path);
        void setSourcePaths(QStringList paths);

        [[nodiscard]] CaseStatus status() const;
        void setStatus(CaseStatus value);

        [[nodiscard]] const QDateTime& createdAtUtc() const;
        void setCreatedAtUtc(QDateTime value);

        [[nodiscard]] const QDateTime& updatedAtUtc() const;
        void setUpdatedAtUtc(QDateTime value);

        [[nodiscard]] const AnalysisResult& analysisResult() const;
        [[nodiscard]] AnalysisResult& analysisResult();
        void setAnalysisResult(AnalysisResult value);

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] QJsonObject toJson() const;
        static CaseRecord fromJson(const QJsonObject& json);

    private:
        ImageFingerprint m_fingerprint;
        QString m_fileName;
        QStringList m_sourcePaths;
        CaseStatus m_status = CaseStatus::New;
        QDateTime m_createdAtUtc;
        QDateTime m_updatedAtUtc;
        AnalysisResult m_analysisResult;
    };
}