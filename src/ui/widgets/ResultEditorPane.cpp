#include "ui/widgets/ResultEditorPane.hpp"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>

#include "domain/CaseAnalysisResult.hpp"

namespace icodental::ui {
    namespace {
        QLabel* createValueLabel(QWidget* parent) {
            auto* label = new QLabel("—", parent);
            label->setWordWrap(true);
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            label->setStyleSheet(
                "QLabel {"
                "  padding: 5px 7px;"
                "  background: #ffffff;"
                "  border: 1px solid #d1d5db;"
                "  border-radius: 4px;"
                "}");
            return label;
        }

        QTextEdit* createReadOnlyTextEdit(QWidget* parent) {
            auto* textEdit = new QTextEdit(parent);
            textEdit->setReadOnly(true);
            textEdit->setMinimumHeight(72);
            textEdit->setStyleSheet(
                "QTextEdit {"
                "  background: #ffffff;"
                "  border: 1px solid #d1d5db;"
                "  border-radius: 4px;"
                "  padding: 4px;"
                "}");
            return textEdit;
        }

        QGroupBox* createGroupBox(const QString& title, QWidget* parent) {
            auto* groupBox = new QGroupBox(title, parent);
            groupBox->setStyleSheet(
                "QGroupBox {"
                "  font-weight: 600;"
                "  border: 1px solid #cbd5e1;"
                "  border-radius: 6px;"
                "  margin-top: 12px;"
                "  padding-top: 8px;"
                "}"
                "QGroupBox::title {"
                "  subcontrol-origin: margin;"
                "  left: 10px;"
                "  padding: 0 4px;"
                "}");
            return groupBox;
        }
    }

    ResultEditorPane::ResultEditorPane(QWidget* parent)
        : QWidget(parent)
    {
        auto* outerLayout = new QVBoxLayout(this);
        outerLayout->setContentsMargins(0, 0, 0, 0);

        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);

        auto* content = new QWidget(scrollArea);
        auto* contentLayout = new QVBoxLayout(content);
        contentLayout->setContentsMargins(12, 12, 12, 12);
        contentLayout->setSpacing(12);

        auto* patientGroup = createGroupBox("Patient and office", content);
        auto* patientLayout = new QFormLayout(patientGroup);
        m_doctorNameValue = createValueLabel(patientGroup);
        m_officeNameValue = createValueLabel(patientGroup);
        m_patientNameValue = createValueLabel(patientGroup);
        patientLayout->addRow("Doctor", m_doctorNameValue);
        patientLayout->addRow("Office", m_officeNameValue);
        patientLayout->addRow("Patient", m_patientNameValue);

        auto* dentalGroup = createGroupBox("Dental case", content);
        auto* dentalLayout = new QFormLayout(dentalGroup);
        m_caseTypeValue = createValueLabel(dentalGroup);
        m_toothNumberValue = createValueLabel(dentalGroup);
        m_shadeValue = createValueLabel(dentalGroup);
        dentalLayout->addRow("Case type", m_caseTypeValue);
        dentalLayout->addRow("Tooth number", m_toothNumberValue);
        dentalLayout->addRow("Shade", m_shadeValue);

        auto* requirementsGroup = createGroupBox("Doctor requirements", content);
        auto* requirementsLayout = new QVBoxLayout(requirementsGroup);
        m_specialInstructionsValue = createReadOnlyTextEdit(requirementsGroup);
        requirementsLayout->addWidget(m_specialInstructionsValue);

        auto* notesGroup = createGroupBox("Confidence and provider notes", content);
        auto* notesLayout = new QFormLayout(notesGroup);
        m_confidenceNoteValue = createReadOnlyTextEdit(notesGroup);
        m_rawProviderTextValue = createReadOnlyTextEdit(notesGroup);
        m_rawProviderTextValue->setMinimumHeight(120);
        notesLayout->addRow("Confidence", m_confidenceNoteValue);
        notesLayout->addRow("Raw provider text", m_rawProviderTextValue);

        contentLayout->addWidget(patientGroup);
        contentLayout->addWidget(dentalGroup);
        contentLayout->addWidget(requirementsGroup);
        contentLayout->addWidget(notesGroup);
        contentLayout->addStretch();

        scrollArea->setWidget(content);
        outerLayout->addWidget(scrollArea);

        clearResult();
    }

    void ResultEditorPane::displayResult(const icodental::domain::CaseAnalysisResult& result) {
        setValue(m_doctorNameValue, result.doctorName());
        setValue(m_officeNameValue, result.officeName());
        setValue(m_patientNameValue, result.patientName());

        setValue(m_caseTypeValue, result.caseType());
        setValue(m_toothNumberValue, result.toothNumber());
        setValue(m_shadeValue, result.shade());

        m_specialInstructionsValue->setPlainText(result.specialInstructions());
        m_confidenceNoteValue->setPlainText(result.confidenceNote());
        m_rawProviderTextValue->setPlainText(result.rawProviderText());
    }

    void ResultEditorPane::clearResult() {
        setValue(m_doctorNameValue, {});
        setValue(m_officeNameValue, {});
        setValue(m_patientNameValue, {});

        setValue(m_caseTypeValue, {});
        setValue(m_toothNumberValue, {});
        setValue(m_shadeValue, {});

        m_specialInstructionsValue->clear();
        m_confidenceNoteValue->clear();
        m_rawProviderTextValue->clear();
    }

    void ResultEditorPane::setValue(QLabel* label, const QString& value) {
        label->setText(value.trimmed().isEmpty() ? "—" : value);
    }
}