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
#include <QSizePolicy>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QItemSelectionModel>

#include <algorithm>

#include "domain/CaseAnalysisResult.hpp"
#include "domain/ProviderType.hpp"
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

        auto* batchActionsRow = new QHBoxLayout;
        auto* analysisSettingsRow = new QHBoxLayout;

        m_openImageButton = new QPushButton("Open image", controlFrame);
        m_analyzeButton = new QPushButton("Analyze", controlFrame);
        m_demoResultButton = new QPushButton("Load demo result", controlFrame);
        m_clearButton = new QPushButton("Clear", controlFrame);

        m_chooseImagesButton =
            new QPushButton("Choose images…", controlFrame);

        m_startBatchButton =
            new QPushButton("Start batch", controlFrame);
        m_startBatchButton->setEnabled(false);

        m_removeSelectedButton =
            new QPushButton("Remove selected", controlFrame);
        m_removeSelectedButton->setEnabled(false);

        m_cancelBatchButton =
            new QPushButton("Cancel batch", controlFrame);
        m_cancelBatchButton->setEnabled(false);

        m_providerComboBox = new QComboBox(controlFrame);
        m_providerComboBox->addItems({"Gemini", "Ollama"});

        m_modelComboBox = new QComboBox(controlFrame);
        m_modelComboBox->setEditable(true);
        m_modelComboBox->addItem("gemini-2.5-flash");

        m_forceRefreshCheckBox =
            new QCheckBox("Force refresh", controlFrame);

        m_chooseImagesButton->setMinimumWidth(120);
        m_startBatchButton->setMinimumWidth(100);
        m_removeSelectedButton->setMinimumWidth(135);
        m_cancelBatchButton->setMinimumWidth(110);

        m_providerComboBox->setMinimumWidth(105);
        m_modelComboBox->setMinimumWidth(180);

        m_forceRefreshCheckBox->setSizePolicy(
            QSizePolicy::Maximum,
            QSizePolicy::Preferred);

        batchActionsRow->addWidget(m_openImageButton);
        batchActionsRow->addWidget(m_chooseImagesButton);
        batchActionsRow->addWidget(m_startBatchButton);
        batchActionsRow->addWidget(m_removeSelectedButton);
        batchActionsRow->addWidget(m_cancelBatchButton);
        batchActionsRow->addWidget(m_clearButton);
        batchActionsRow->addStretch();

        analysisSettingsRow->addWidget(
            new QLabel("Provider", controlFrame));
        analysisSettingsRow->addWidget(m_providerComboBox);

        analysisSettingsRow->addWidget(
            new QLabel("Model", controlFrame));
        analysisSettingsRow->addWidget(m_modelComboBox, 1);

        analysisSettingsRow->addWidget(m_forceRefreshCheckBox);
        analysisSettingsRow->addWidget(m_demoResultButton);
        analysisSettingsRow->addWidget(m_analyzeButton);

        auto* secondRow = new QHBoxLayout;
        secondRow->addWidget(new QLabel("Optional note", controlFrame));
        m_optionalPromptLineEdit = new QLineEdit(controlFrame);
        m_optionalPromptLineEdit->setPlaceholderText(
            "Optional instruction for the analysis, such as “focus on the shade and tooth number”");
        secondRow->addWidget(m_optionalPromptLineEdit, 1);

        controlLayout->addLayout(batchActionsRow);
        controlLayout->addLayout(analysisSettingsRow);
        controlLayout->addLayout(secondRow);

        auto* batchLayout = new QHBoxLayout;
        m_batchProgressBar = new QProgressBar(controlFrame);
        m_batchProgressBar->setRange(0, 1);
        m_batchProgressBar->setValue(0);
        m_batchProgressBar->setTextVisible(true);
        m_batchProgressBar->setFormat("No batch running");
        batchLayout->addWidget(new QLabel("Batch progress", controlFrame));
        batchLayout->addWidget(m_batchProgressBar, 1);
        controlLayout->addLayout(batchLayout);

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

        m_batchTable = new QTableWidget(centralWidget);
        m_batchTable->setColumnCount(5);
        m_batchTable->setHorizontalHeaderLabels({
            "File",
            "Status",
            "Provider",
            "Model",
            "Message"
        });
        m_batchTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_batchTable->setSelectionBehavior(
            QAbstractItemView::SelectRows);
        m_batchTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_batchTable->horizontalHeader()->setStretchLastSection(true);
        m_batchTable->setVisible(false);
        rootLayout->addWidget(m_batchTable);

        setCentralWidget(centralWidget);

        m_statusLabel = new QLabel("Ready — select a prescription image.", this);
        statusBar()->addWidget(m_statusLabel, 1);

        connect(m_openImageButton, &QPushButton::clicked, this, [this] {
            openImage();
        });

        connect(m_chooseImagesButton, &QPushButton::clicked, this, [this] {
            chooseImages();
        });

        connect(m_startBatchButton, &QPushButton::clicked, this, [this] { 
            startBatch(); 
        });

        connect(m_cancelBatchButton, &QPushButton::clicked, this, [this] { 
            cancelBatch();
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

        connect(
            &m_viewModel,
            &MainViewModel::analysisStarted,
            this,
            [this](const QString& message) {
                setAnalysisControlsEnabled(false);
                m_statusLabel->setText(message);
        });

        connect(
            &m_viewModel,
            &MainViewModel::analysisSucceeded,
            this,
            [this](
                const icodental::domain::CaseAnalysisResult& result,
                const QString& message) {
                setAnalysisControlsEnabled(true);
                m_resultEditorPane->displayResult(result);
                m_statusLabel->setText(message);
        });

        connect(
            &m_viewModel,
            &MainViewModel::analysisFailed,
            this,
            [this](const QString& message) {
                setAnalysisControlsEnabled(true);
                m_statusLabel->setText("Analysis failed.");

                QMessageBox::warning(
                    this,
                    "Analysis failed",
                    message);
        });

        connect(
            m_providerComboBox,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString& providerName) {
                m_modelComboBox->clear();

                if (providerName.compare("Ollama", Qt::CaseInsensitive) == 0) {
                    m_modelComboBox->addItems({
                        "gemma4:e4b",
                        "llama3.2-vision:latest"
                    });
                } else {
                    m_modelComboBox->addItems({
                        "gemini-2.5-flash"
                    });
                }
        });

        connect(
            &m_viewModel,
            &MainViewModel::batchAnalysisStarted,
            this,
            [this](int totalCount) {
                m_batchProgressBar->setRange(0, totalCount);
                m_batchProgressBar->setValue(0);
                m_batchProgressBar->setFormat(
                    QString("Analyzing 0 of %1").arg(totalCount));
                refreshBatchTable();
        });

        connect(
            &m_viewModel,
            &MainViewModel::batchAnalysisItemUpdated,
            this,
            [this](int updatedRow) {
                refreshBatchTable();

                if (updatedRow == m_currentBatchRow) {
                    reviewBatchRow(updatedRow);
                }
        });

        connect(
            &m_viewModel,
            &MainViewModel::batchAnalysisProgressChanged,
            this,
            &MainWindow::updateBatchProgress);

        connect(
            &m_viewModel,
            &MainViewModel::batchAnalysisFinished,
            this,
            &MainWindow::showBatchFinished);

        connect(
            &m_viewModel,
            &MainViewModel::batchAnalysisFailedToStart,
            this,
            [this](const QString& message) {
                setBatchControlsRunning(false);

                QMessageBox::warning(
                    this,
                    "Batch analysis could not start",
                    message);
        });

        connect(
            m_batchTable,
            &QTableWidget::cellClicked,
            this,
            [this](int row, int) {
                m_currentBatchRow = row;
                reviewBatchRow(row);
            }
        );

        connect(
            m_removeSelectedButton,
            &QPushButton::clicked,
            this,
            [this] { removeSelectedBatchRows(); });

        connect(
            m_batchTable->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this] {
                updateBatchActionButtons();
        });

        emit m_providerComboBox->currentTextChanged(
            m_providerComboBox->currentText());
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

    void MainWindow::chooseImages() {
        const QStringList imagePaths = QFileDialog::getOpenFileNames(
            this,
            "Choose prescription images",
            {},
            "Images (*.png *.jpg *.jpeg *.bmp *.webp *.tif *.tiff)");

        if (imagePaths.isEmpty()) {
            return;
        }

        m_batchImagePaths = imagePaths;
        m_currentBatchRow = -1;

        m_batchTable->setVisible(true);
        m_batchTable->setRowCount(imagePaths.size());

        for (int row = 0; row < imagePaths.size(); ++row) {
            const QString& imagePath = imagePaths.at(row);

            m_batchTable->setItem(
                row,
                0,
                new QTableWidgetItem(
                    QFileInfo(imagePath).fileName()));

            m_batchTable->setItem(
                row,
                1,
                new QTableWidgetItem("Ready"));

            m_batchTable->setItem(
                row,
                2,
                new QTableWidgetItem(
                    m_providerComboBox->currentText()));

            m_batchTable->setItem(
                row,
                3,
                new QTableWidgetItem(
                    m_modelComboBox->currentText()));

            m_batchTable->setItem(
                row,
                4,
                new QTableWidgetItem(
                    m_forceRefreshCheckBox->isChecked()
                        ? "Will be reanalyzed."
                        : "Will use cache when available."));
        }

        m_batchTable->resizeColumnsToContents();

        m_batchProgressBar->setRange(0, imagePaths.size());
        m_batchProgressBar->setValue(0);
        m_batchProgressBar->setFormat(
            QString("%1 image(s) ready to analyze.")
                .arg(imagePaths.size()));

        m_startBatchButton->setEnabled(true);
        m_cancelBatchButton->setEnabled(false);

        m_statusLabel->setText(
            QString("%1 image(s) selected. Review settings, then start batch.")
                .arg(imagePaths.size()));

        m_batchTable->selectRow(0);
        m_currentBatchRow = 0;
        reviewBatchRow(m_currentBatchRow);
    }

    void MainWindow::startBatch() {
        if (m_batchImagePaths.isEmpty()) {
            QMessageBox::information(
                this,
                "Choose images first",
                "Choose one or more prescription images before starting a batch.");
            return;
        }

        setBatchControlsRunning(true);

        m_viewModel.analyzeBatch(
            m_batchImagePaths,
            m_providerComboBox->currentText(),
            m_modelComboBox->currentText(),
            m_optionalPromptLineEdit->text(),
            m_forceRefreshCheckBox->isChecked());
    }

    void MainWindow::setBatchControlsRunning(bool running) {
        m_openImageButton->setEnabled(!running);
        m_chooseImagesButton->setEnabled(!running);
        m_startBatchButton->setEnabled(!running && !m_batchImagePaths.isEmpty());
        m_analyzeButton->setEnabled(!running);
        m_clearButton->setEnabled(!running);

        m_providerComboBox->setEnabled(!running);
        m_modelComboBox->setEnabled(!running);
        m_optionalPromptLineEdit->setEnabled(!running);
        m_forceRefreshCheckBox->setEnabled(!running);

        m_cancelBatchButton->setEnabled(running);
    }

    void MainWindow::clearScreen() {
        m_selectedImagePath.clear();
        m_imagePreviewPane->clearImage();
        m_resultEditorPane->clearResult();
        m_optionalPromptLineEdit->clear();
        m_forceRefreshCheckBox->setChecked(false);
        m_statusLabel->setText("Ready — select a prescription image.");
        m_currentBatchRow = -1;
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

        m_viewModel.analyzeSingleImage(
            m_selectedImagePath,
            m_providerComboBox->currentText(),
            m_modelComboBox->currentText().trimmed(),
            m_optionalPromptLineEdit->text(),
            m_forceRefreshCheckBox->isChecked());
    }

    void MainWindow::setAnalysisControlsEnabled(bool enabled) {
        m_openImageButton->setEnabled(enabled);
        m_clearButton->setEnabled(enabled);
        m_analyzeButton->setEnabled(enabled);
        m_demoResultButton->setEnabled(enabled);

        m_providerComboBox->setEnabled(enabled);
        m_modelComboBox->setEnabled(enabled);
        m_forceRefreshCheckBox->setEnabled(enabled);
        m_optionalPromptLineEdit->setEnabled(enabled);
    }

    namespace {
        QString batchStateText(BatchItemState state) {
            switch (state) {
            case BatchItemState::Pending:
                return "Pending";
            case BatchItemState::Cached:
                return "Cached";
            case BatchItemState::Running:
                return "Analyzing";
            case BatchItemState::Succeeded:
                return "Completed";
            case BatchItemState::Failed:
                return "Failed";
            case BatchItemState::Cancelled:
                return "Cancelled";
            }

            return "Unknown";
        }

        QString providerText(icodental::domain::ProviderType provider) {
            switch (provider) {
            case icodental::domain::ProviderType::Gemini:
                return "Gemini";
            case icodental::domain::ProviderType::Ollama:
                return "Ollama";
            case icodental::domain::ProviderType::Unknown:
                return "Unknown";
            }

            return "Unknown";
        }
    }

    void MainWindow::refreshBatchTable() {
        const QList<BatchAnalysisItem>& items = m_viewModel.batchController().items();

        m_batchTable->setRowCount(items.size());

        for (int row = 0; row < items.size(); ++row) {
            const BatchAnalysisItem& item = items.at(row);

            m_batchTable->setItem(
                row,
                0,
                new QTableWidgetItem(
                    QFileInfo(item.imagePath).fileName()));

            m_batchTable->setItem(
                row,
                1,
                new QTableWidgetItem(
                    batchStateText(item.state)));

            m_batchTable->setItem(
                row,
                2,
                new QTableWidgetItem(
                    providerText(item.provider)));

            m_batchTable->setItem(
                row,
                3,
                new QTableWidgetItem(item.model));

            m_batchTable->setItem(
                row,
                4,
                new QTableWidgetItem(item.message));
        }

        m_batchTable->resizeColumnsToContents();
    }

    void MainWindow::updateBatchProgress(
        int completedCount,
        int totalCount)
    {
        m_batchProgressBar->setRange(0, totalCount);
        m_batchProgressBar->setValue(completedCount);
        m_batchProgressBar->setFormat(
            QString("Analyzing %1 of %2")
                .arg(completedCount)
                .arg(totalCount));
    }

    void MainWindow::cancelBatch() {
        m_cancelBatchButton->setEnabled(false);
        m_batchProgressBar->setFormat("Cancelling after current image…");
        m_viewModel.cancelBatchAnalysis();
    }

    void MainWindow::showBatchFinished(
        int succeededCount,
        int failedCount,
        int cancelledCount)
    {
        setBatchControlsRunning(false);
        m_startBatchButton->setEnabled(false);

        const int totalCount =
            succeededCount + failedCount + cancelledCount;

        m_batchProgressBar->setRange(0, totalCount);
        m_batchProgressBar->setValue(totalCount);
        m_batchProgressBar->setFormat(
            QString("Finished: %1 succeeded, %2 failed, %3 cancelled")
                .arg(succeededCount)
                .arg(failedCount)
                .arg(cancelledCount));

        refreshBatchTable();

        m_statusLabel->setText(
            QString("Batch finished: %1 succeeded, %2 failed, %3 cancelled. "
                    "Select a row to review its image and result.")
                .arg(succeededCount)
                .arg(failedCount)
                .arg(cancelledCount));
    }

    void MainWindow::removeSelectedBatchRows() {
        if (m_viewModel.batchController().isRunning()) {
            return;
        }

        QModelIndexList selectedRows =
            m_batchTable->selectionModel()->selectedRows();

        if (selectedRows.isEmpty()) {
            return;
        }

        std::sort(
            selectedRows.begin(),
            selectedRows.end(),
            [](const QModelIndex& left, const QModelIndex& right) {
                return left.row() > right.row();
            });

        for (const QModelIndex& index : selectedRows) {
            const int row = index.row();

            if (row >= 0 && row < m_batchImagePaths.size()) {
                m_batchImagePaths.removeAt(row);
            }

            m_batchTable->removeRow(row);
        }

        if (m_batchImagePaths.isEmpty()) {
            m_startBatchButton->setEnabled(false);

            m_batchProgressBar->setRange(0, 1);
            m_batchProgressBar->setValue(0);
            m_batchProgressBar->setFormat("No images selected.");

            m_statusLabel->setText(
                "No batch images selected.");
        } else {
            m_batchProgressBar->setRange(
                0,
                m_batchImagePaths.size());

            m_batchProgressBar->setValue(0);
            m_batchProgressBar->setFormat(
                QString("%1 image(s) ready to analyze.")
                    .arg(m_batchImagePaths.size()));

            m_statusLabel->setText(
                QString("%1 image(s) selected. Review settings, then start batch.")
                    .arg(m_batchImagePaths.size()));
        }

        updateBatchActionButtons();
    }

    void MainWindow::updateBatchActionButtons() {
        const bool batchRunning = m_viewModel.batchController().isRunning();

        const bool hasSelection =
            m_batchTable->selectionModel() != nullptr
            && !m_batchTable->selectionModel()
                    ->selectedRows()
                    .isEmpty();

        const bool hasBatchImages = !m_batchImagePaths.isEmpty();

        m_removeSelectedButton->setEnabled(!batchRunning && hasSelection);

        m_startBatchButton->setEnabled(!batchRunning && hasBatchImages);
    }

    void MainWindow::reviewBatchRow(int row) {
        if (row < 0 || row >= m_batchImagePaths.size()) {
            return;
        }

        const QString& imagePath = m_batchImagePaths.at(row);

        if (!m_imagePreviewPane->loadImage(imagePath)) {
            QMessageBox::warning(
                this,
                "Unable to open image",
                QString("Could not load: %1").arg(imagePath));
            return;
        }

        m_selectedImagePath = imagePath;

        const auto& items = m_viewModel.batchController().items();

        if (row < items.size()) {
            const BatchAnalysisItem& item = items.at(row);

            if (item.result.has_value()) {
                m_resultEditorPane->displayResult(item.result.value());
            } else {
                m_resultEditorPane->clearResult();
            }

            m_statusLabel->setText(
                QString("%1 — %2")
                    .arg(QFileInfo(imagePath).fileName())
                    .arg(item.message));
            return;
        }

        m_resultEditorPane->clearResult();

        m_statusLabel->setText(
            QString("%1 — Ready to analyze.")
                .arg(QFileInfo(imagePath).fileName()));
    }
}