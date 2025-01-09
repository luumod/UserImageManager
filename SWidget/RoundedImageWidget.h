#ifndef ROUNDEDIMAGEWIDGET_H
#define ROUNDEDIMAGEWIDGET_H

#include <QWidget>
#include <QPixmap>

class RoundedImageWidget : public QWidget
{
public:
    RoundedImageWidget(const QPixmap& pixmap,int w,int h, QWidget* parent = 0);
    ~RoundedImageWidget();
protected:
    void paintEvent(QPaintEvent* event)override;
private:
    QPixmap m_pixmap;

    int m_w;
    int m_h;
};

#endif // ROUNDEDIMAGEWIDGET_H