#include "CaseRecord.hpp"

namespace icodental::domain {
    const ImageFingerprint& CaseRecord::fingerprint() const { return m_fingerprint; }
    void CaseRecord::setFingerprint(ImageFingerprint value) { m_fingerprint = std::move(value); }

    const QString& CaseRecord::fileName() const { return m_fileName; }
    void CaseRecord::setFileName(QString value) { m_fileName = std::move(value); }

    const QStringList& CaseRecord::sourcePaths() const { return m_sourcePaths; }

    void CaseRecord::addSourcePath(const QString& path) {
        if (!path.isEmpty() && !m_sourcePaths.contains(path)) {
            m_sourcePaths.append(path);
        }
    }

    void CaseRecord::setSourcePaths(QStringList paths) {
        m_sourcePaths = std::move(paths);
        m_sourcePaths.removeDuplicates();
    }

    CaseStatus CaseRecord::status() const { return m_status; }
    void CaseRecord::setStatus(CaseStatus value) { m_status = value; }

    const QDateTime& CaseRecord::createdAtUtc() const { return m_createdAtUtc; }
    void CaseRecord::setCreatedAtUtc(QDateTime value) { m_createdAtUtc = std::move(value); }

    const QDateTime& CaseRecord::updatedAtUtc() const { return m_updatedAtUtc; }
    void CaseRecord::setUpdatedAtUtc(QDateTime value) { m_updatedAtUtc =
        std::move(value); }

    const AnalysisResult& CaseRecord::analysisResult() const { return m_analysisResult; }
    AnalysisResult& CaseRecord::analysisResult() { return m_analysisResult; }
    void CaseRecord::setAnalysisResult(AnalysisResult value) { m_analysisResult = std::move(value); }

    bool CaseRecord::isValid() const {
        return m_fingerprint.isValid() && !m_fileName.trimmed().isEmpty();
    }


    QJsonObject CaseRecord::toJson() const {
        QJsonArray sourcePathsJson;
        for (const auto& path : m_sourcePaths) {
            sourcePathsJson.append(path);
        }

        return QJsonObject{
            {"fingerprint", m_fingerprint.toJson()},
            {"file_name", m_fileName},
            {"source_paths", sourcePathsJson},
            {"status", toString(m_status)},
            {"created_at_utc", m_createdAtUtc.toUTC().toString(Qt::ISODate)},
            {"updated_at_utc", m_updatedAtUtc.toUTC().toString(Qt::ISODate)},
            {"analysis_result", m_analysisResult.toJson()}
        };
    }

    CaseRecord CaseRecord::fromJson(const QJsonObject& json) {
        CaseRecord record;
        record.setFingerprint(ImageFingerprint::fromJson(json.value("fingerprint").toObject()));
        record.setFileName(json.value("file_name").toString());

        QStringList sourcePaths;
        const auto sourceArray = json.value("source_paths").toArray();
        for (const auto& value : sourceArray) {
            sourcePaths.append(value.toString());
        }
        record.setSourcePaths(std::move(sourcePaths));

        const auto status = caseStatusFromString(json.value("status").toString());
        record.setStatus(status.value_or(CaseStatus::Error));

        record.setCreatedAtUtc(QDateTime::fromString(json.value("created_at_utc").toString(), Qt::ISODate));
        record.setUpdatedAtUtc(QDateTime::fromString(json.value("updated_at_utc").toString(), Qt::ISODate));
        record.setAnalysisResult(AnalysisResult::fromJson(json.value("analysis_result").toObject()));

        return record;
    }
}