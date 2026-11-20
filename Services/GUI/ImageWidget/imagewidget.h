#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class ImageWidget : public QLabel
{
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    bool setImage(const QString& image_path);
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    void updateImage();
    QPixmap original_pixmap_;
};
#endif // IMAGEWIDGET_H
