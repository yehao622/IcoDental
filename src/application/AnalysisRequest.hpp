#pragma once

#include <QList>
#include <QString>

#include "domain/ProviderType.hpp"

namespace icodental::application {
    class AnalysisRequest {
        public:
            enum class InputMode {
                Directory,
                ExplicitFiles
            };

            AnalysisRequest(
                InputMode inputMode,
                QString inputPath,
                QList<QString> explicitFiles,
                domain::ProviderType provider,
                QString model,
                bool forceRefresh);

            [[nodiscard]] InputMode inputMode() const;
            [[nodiscard]] const QString& inputPath() const;
            [[nodiscard]] const QList<QString>& explicitFiles() const;
            [[nodiscard]] domain::ProviderType provider() const;
            [[nodiscard]] const QString& model() const;
            [[nodiscard]] bool forceRefresh() const;
            [[nodiscard]] bool isValid() const;

    private:
        InputMode m_inputMode;
        QString m_inputPath;
        QList<QString> m_explicitFiles;
        domain::ProviderType m_provider;
        QString m_model;
        bool m_forceRefresh;
    };
}