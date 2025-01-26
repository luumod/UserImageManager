#ifndef SDISPLAYIMAGESSCROLLAREAWIDGET_H_
#define SDISPLAYIMAGESSCROLLAREAWIDGET_H_

#include <QWidget>
#include <QPoint>

class QScrollArea;
class SDisplayImagesScrollAreaWidget : public QWidget {
    Q_OBJECT

public:
    SDisplayImagesScrollAreaWidget(QWidget* parent = nullptr);
    ~SDisplayImagesScrollAreaWidget();

    void init();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private slots:
    void moveLeft();
    void moveRight();
    void startAutoScroll();
    void autoScroll();
private:
    QScrollArea* scrollArea;
    bool dragging = false;
    QPoint lastMousePos;

    bool stopScrolling{ false };

    //显示最热的n张图片
    static const int MAX_SHOW_IMAGE_COUNT = 6;
};


#endif // !SDISPLAYIMAGESSCROLLAREAWIDGET_H_