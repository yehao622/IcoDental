#pragma once

#include <QWidget>

class QLabel;
class QPixmap;
class QScrollArea;

namespace icodental::ui {
    class ImagePreviewPane final : public QWidget {
    public:
        explicit ImagePreviewPane(QWidget* parent = nullptr);

        [[nodiscard]] bool loadImage(const QString& imagePath);
        void clearImage();

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        void updatePreview();

        QScrollArea* m_scrollArea{nullptr};
        QLabel* m_imageLabel{nullptr};
        QPixmap* m_originalPixmap{nullptr};
    };
}