#ifndef SUPLOADORDRAGIMAGEWIDGET_H
#define SUPLOADORDRAGIMAGEWIDGET_H

#include <QWidget>

class SUploadorDragImageWidget : public QWidget
{
    Q_OBJECT
public:
    SUploadorDragImageWidget(QWidget *parent = nullptr);
    ~SUploadorDragImageWidget();

    void init();
    bool isImageFile(const QString& filePath);
protected:
    void paintEvent(QPaintEvent *event)override;
    void mouseMoveEvent(QMouseEvent* event)override;
    void leaveEvent(QEvent* event)override;
    void mouseReleaseEvent(QMouseEvent* event)override;
    void dragEnterEvent(QDragEnterEvent *event)override;
    void dropEvent(QDropEvent *event)override;

signals:
    void openFileDialog();
public slots:

private:
    QRect m_InsideRect;

    QImage m_image;

    bool isDraging{};
    bool isMouseInside{};
};

#endif // SUPLOADORDRAGIMAGEWIDGET_H