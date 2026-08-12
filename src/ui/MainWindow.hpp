#pragma once

#include <QMainWindow>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace icodental::ui {
    class ImagePreviewPane;
    class ResultEditorPane;

    class MainWindow final : public QMainWindow {
    public:
        explicit MainWindow(QWidget* parent = nullptr);

    private:
        void openImage();
        void clearScreen();
        void showDemoResult();
        void analyzeCurrentImage();

        ImagePreviewPane* m_imagePreviewPane{nullptr};
        ResultEditorPane* m_resultEditorPane{nullptr};

        QPushButton* m_openImageButton{nullptr};
        QPushButton* m_analyzeButton{nullptr};
        QPushButton* m_demoResultButton{nullptr};
        QPushButton* m_clearButton{nullptr};

        QComboBox* m_providerComboBox{nullptr};
        QComboBox* m_modelComboBox{nullptr};
        QCheckBox* m_forceRefreshCheckBox{nullptr};
        QLineEdit* m_optionalPromptLineEdit{nullptr};
        QLabel* m_statusLabel{nullptr};

        QString m_selectedImagePath;
    };
}