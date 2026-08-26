#pragma once

#include <QMainWindow>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QProgressBar>
#include <QTableWidget>
#include <QStringList>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace icodental::ui {
    class ImagePreviewPane;
    class ResultEditorPane;
    class MainViewModel;

    class MainWindow final : public QMainWindow {
    public:
        explicit MainWindow(MainViewModel& viewModel, QWidget* parent = nullptr);

    private:
        void openImage();
        void clearScreen();
        void showDemoResult();
        void analyzeCurrentImage();
        void setAnalysisControlsEnabled(bool enabled);
        void chooseImages();
        void startBatch();
        void cancelBatch();
        void setBatchControlsRunning(bool running);
        void refreshBatchTable();
        void updateBatchProgress(int completedCount, int totalCount);
        void showBatchFinished(int succeededCount, int failedCount, int cancelledCount);

        ImagePreviewPane* m_imagePreviewPane{nullptr};
        ResultEditorPane* m_resultEditorPane{nullptr};

        QPushButton* m_openImageButton{nullptr};
        QPushButton* m_analyzeButton{nullptr};
        QPushButton* m_demoResultButton{nullptr};
        QPushButton* m_clearButton{nullptr};
        QPushButton* m_chooseImagesButton{nullptr};
        QPushButton* m_startBatchButton{nullptr};
        QPushButton* m_cancelBatchButton{nullptr};
        QProgressBar* m_batchProgressBar{nullptr};
        QTableWidget* m_batchTable{nullptr};

        QStringList m_batchImagePaths;
        QComboBox* m_providerComboBox{nullptr};
        QComboBox* m_modelComboBox{nullptr};
        QCheckBox* m_forceRefreshCheckBox{nullptr};
        QLineEdit* m_optionalPromptLineEdit{nullptr};
        QLabel* m_statusLabel{nullptr};

        MainViewModel& m_viewModel;
        QString m_selectedImagePath;
    };
}