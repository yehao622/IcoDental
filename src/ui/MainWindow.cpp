#include "ui/MainWindow.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "domain/CaseAnalysisResult.hpp"
#include "ui/widgets/ImagePreviewPane.hpp"
#include "ui/widgets/ResultEditorPane.hpp"
#include "ui/viewmodels/MainViewModel.hpp"

namespace icodental::ui {
    MainWindow::MainWindow(
        MainViewModel& viewModel,
        QWidget* parent)
        : QMainWindow(parent)
        , m_viewModel(viewModel)
    {
        setWindowTitle("IcoDental");
        resize(1440, 900);
        setMinimumSize(1050, 680);

        auto* centralWidget = new QWidget(this);
        auto* rootLayout = new QVBoxLayout(centralWidget);
        rootLayout->setContentsMargins(12, 12, 12, 12);
        rootLayout->setSpacing(10);

        auto* controlFrame = new QFrame(centralWidget);
        controlFrame->setFrameShape(QFrame::StyledPanel);
        auto* controlLayout = new QVBoxLayout(controlFrame);
        controlLayout->setContentsMargins(12, 12, 12, 12);
        controlLayout->setSpacing(8);

        auto* firstRow = new QHBoxLayout;
        m_openImageButton = new QPushButton("Open image", controlFrame);
        m_analyzeButton = new QPushButton("Analyze", controlFrame);
        m_demoResultButton = new QPushButton("Load demo result", controlFrame);
        m_clearButton = new QPushButton("Clear", controlFrame);

        m_providerComboBox = new QComboBox(controlFrame);
        m_providerComboBox->addItems({"Gemini", "Ollama"});

        m_modelComboBox = new QComboBox(controlFrame);
        m_modelComboBox->setEditable(true);
        m_modelComboBox->addItems({"gemini-2.5-flash", "gemini-2.5-pro", "llava"});

        m_forceRefreshCheckBox = new QCheckBox("Force refresh", controlFrame);

        firstRow->addWidget(m_openImageButton);
        firstRow->addWidget(m_clearButton);
        firstRow->addSpacing(12);
        firstRow->addWidget(new QLabel("Provider", controlFrame));
        firstRow->addWidget(m_providerComboBox);
        firstRow->addWidget(new QLabel("Model", controlFrame));
        firstRow->addWidget(m_modelComboBox, 1);
        firstRow->addWidget(m_forceRefreshCheckBox);
        firstRow->addStretch();
        firstRow->addWidget(m_demoResultButton);
        firstRow->addWidget(m_analyzeButton);

        auto* secondRow = new QHBoxLayout;
        secondRow->addWidget(new QLabel("Optional note", controlFrame));
        m_optionalPromptLineEdit = new QLineEdit(controlFrame);
        m_optionalPromptLineEdit->setPlaceholderText(
            "Optional instruction for the analysis, such as “focus on the shade and tooth number”");
        secondRow->addWidget(m_optionalPromptLineEdit, 1);

        controlLayout->addLayout(firstRow);
        controlLayout->addLayout(secondRow);

        auto* splitter = new QSplitter(Qt::Horizontal, centralWidget);

        auto* imagePanel = new QWidget(splitter);
        auto* imageLayout = new QVBoxLayout(imagePanel);
        imageLayout->setContentsMargins(0, 0, 0, 0);
        auto* imageTitle = new QLabel("Prescription preview", imagePanel);
        imageTitle->setStyleSheet("font-weight: 600; padding: 4px;");
        m_imagePreviewPane = new ImagePreviewPane(imagePanel);
        imageLayout->addWidget(imageTitle);
        imageLayout->addWidget(m_imagePreviewPane, 1);

        auto* resultPanel = new QWidget(splitter);
        auto* resultLayout = new QVBoxLayout(resultPanel);
        resultLayout->setContentsMargins(0, 0, 0, 0);
        auto* resultTitle = new QLabel("Extracted information", resultPanel);
        resultTitle->setStyleSheet("font-weight: 600; padding: 4px;");
        m_resultEditorPane = new ResultEditorPane(resultPanel);
        resultLayout->addWidget(resultTitle);
        resultLayout->addWidget(m_resultEditorPane, 1);

        splitter->addWidget(imagePanel);
        splitter->addWidget(resultPanel);
        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 2);
        splitter->setSizes({850, 550});

        rootLayout->addWidget(controlFrame);
        rootLayout->addWidget(splitter, 1);

        setCentralWidget(centralWidget);

        m_statusLabel = new QLabel("Ready — select a prescription image.", this);
        statusBar()->addWidget(m_statusLabel, 1);

        connect(m_openImageButton, &QPushButton::clicked, this, [this] {
            openImage();
        });

        connect(m_clearButton, &QPushButton::clicked, this, [this] {
            clearScreen();
        });

        connect(m_demoResultButton, &QPushButton::clicked, this, [this] {
            showDemoResult();
        });

        connect(m_analyzeButton, &QPushButton::clicked, this, [this] {
            analyzeCurrentImage();
        });
    }

    void MainWindow::openImage() {
        const QString imagePath = QFileDialog::getOpenFileName(
            this,
            "Choose prescription image",
            {},
            "Images (*.png *.jpg *.jpeg *.bmp *.webp *.tif *.tiff)");

        if (imagePath.isEmpty()) {
            return;
        }

        if (!m_imagePreviewPane->loadImage(imagePath)) {
            QMessageBox::warning(
                this,
                "Unable to open image",
                "The selected file could not be loaded as an image.");
            return;
        }

        m_selectedImagePath = imagePath;
        m_resultEditorPane->clearResult();
        m_statusLabel->setText(
            QString("Loaded: %1").arg(QFileInfo(imagePath).fileName()));
    }

    void MainWindow::clearScreen() {
        m_selectedImagePath.clear();
        m_imagePreviewPane->clearImage();
        m_resultEditorPane->clearResult();
        m_optionalPromptLineEdit->clear();
        m_forceRefreshCheckBox->setChecked(false);
        m_statusLabel->setText("Ready — select a prescription image.");
    }

    void MainWindow::showDemoResult() {
        const icodental::domain::CaseAnalysisResult demoResult(
            "Dr. Elena Carter",
            "Northside Dental Studio",
            "Alex Morgan",
            "Ceramic crown",
            "36",
            "A2",
            "Match the adjacent teeth. Keep the occlusal anatomy subtle and preserve the existing contact points.",
            "High confidence. Please verify the final shade under the clinic’s lighting conditions.",
            "Demo provider response. This will be replaced by the real provider response after AI wiring.");

        m_resultEditorPane->displayResult(demoResult);
        m_statusLabel->setText("Showing a demonstration result.");
    }

    void MainWindow::analyzeCurrentImage() {
        if (m_selectedImagePath.isEmpty()) {
            QMessageBox::information(
                this,
                "Select an image first",
                "Choose a prescription image before starting analysis.");
            return;
        }

        const auto outcome = m_viewModel.analyzeSingleImage(
            m_selectedImagePath,
            m_providerComboBox->currentText(),
            m_modelComboBox->currentText().trimmed(),
            m_forceRefreshCheckBox->isChecked());

        switch (outcome.status) {
            case MainViewModel::AnalyzeStatus::CacheHit:
                m_resultEditorPane->displayResult(
                    outcome.caseAnalysisResult.value());
                m_statusLabel->setText(outcome.message);
                return;

            case MainViewModel::AnalyzeStatus::ProviderAnalysisRequired:
                m_statusLabel->setText(outcome.message);

                QMessageBox::information(
                    this,
                    "Provider analysis required",
                    outcome.message
                        + "\n\n"
                          "Gemini analysis will be connected in the next implementation step.");
                return;

            case MainViewModel::AnalyzeStatus::Error:
                m_statusLabel->setText("Analysis planning failed.");

                QMessageBox::warning(
                    this,
                    "Unable to prepare analysis",
                    outcome.message);
                return;
        }

        showDemoResult();
        m_statusLabel->setText(
            "Demo analysis completed. Real provider integration is the next step.");
    }
}