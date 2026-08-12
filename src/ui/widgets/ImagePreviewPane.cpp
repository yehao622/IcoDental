#include "ui/widgets/ImagePreviewPane.hpp"

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollArea>
#include <QVBoxLayout>

namespace icodental::ui {

    ImagePreviewPane::ImagePreviewPane(QWidget* parent)
        : QWidget(parent)
        , m_originalPixmap(new QPixmap)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        m_scrollArea = new QScrollArea(this);
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setAlignment(Qt::AlignCenter);

        m_imageLabel = new QLabel(m_scrollArea);
        m_imageLabel->setAlignment(Qt::AlignCenter);
        m_imageLabel->setMinimumSize(300, 300);
        m_imageLabel->setText("No prescription image selected");
        m_imageLabel->setStyleSheet(
            "QLabel {"
            "  color: #6b7280;"
            "  background: #fff8ed;"
            "  border: 1px solid #d1d5db;"
            "  border-radius: 6px;"
            "}");

        m_scrollArea->setWidget(m_imageLabel);
        layout->addWidget(m_scrollArea);
    }

    bool ImagePreviewPane::loadImage(const QString& imagePath) {
        QPixmap loadedPixmap(imagePath);
        if (loadedPixmap.isNull()) {
            return false;
        }

        *m_originalPixmap = loadedPixmap;
        updatePreview();
        return true;
    }

    void ImagePreviewPane::clearImage() {
        *m_originalPixmap = QPixmap();
        m_imageLabel->setPixmap(QPixmap());
        m_imageLabel->setText("No prescription image selected");
    }

    void ImagePreviewPane::resizeEvent(QResizeEvent* event) {
        QWidget::resizeEvent(event);
        updatePreview();
    }

    void ImagePreviewPane::updatePreview() {
        if (m_originalPixmap->isNull()) {
            return;
        }

        const QSize availableSize = m_scrollArea->viewport()->size() - QSize(24, 24);
        const QPixmap scaledPixmap = m_originalPixmap->scaled(
            availableSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);

        m_imageLabel->setText({});
        m_imageLabel->setPixmap(scaledPixmap);
        m_imageLabel->resize(scaledPixmap.size());
    }
}