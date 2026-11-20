#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget *parent)
        : QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
}

bool ImageWidget::setImage(const QString &image_path)
{
    QPixmap pixmap;
    if (!pixmap.load(image_path)) {
        return false;
    }

    original_pixmap_ = pixmap;
    updateImage();

    return true;
}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);

    updateImage();
}

void ImageWidget::updateImage()
{
    if (original_pixmap_.isNull()) {
        clear();
        return;
    }


    setPixmap(original_pixmap_.scaled(
       size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    ));
}
