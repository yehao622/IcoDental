#pragma once

#include <QWidget>

namespace icodental::domain {
class CaseAnalysisResult;
}

class QLabel;
class QTextEdit;

namespace icodental::ui {
    class ResultEditorPane final : public QWidget {
    public:
        explicit ResultEditorPane(QWidget* parent = nullptr);

        void displayResult(const icodental::domain::CaseAnalysisResult& result);
        void clearResult();

    private:
        static void setValue(QLabel* label, const QString& value);

        QLabel* m_doctorNameValue{nullptr};
        QLabel* m_officeNameValue{nullptr};
        QLabel* m_patientNameValue{nullptr};

        QLabel* m_caseTypeValue{nullptr};
        QLabel* m_toothNumberValue{nullptr};
        QLabel* m_shadeValue{nullptr};

        QTextEdit* m_specialInstructionsValue{nullptr};
        QTextEdit* m_confidenceNoteValue{nullptr};
        QTextEdit* m_rawProviderTextValue{nullptr};
    };
}