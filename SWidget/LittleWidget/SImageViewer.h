#ifndef SIMAGEVIEWER_H
#define SIMAGEVIEWER_H

#include <QMainWindow>

class QLabel;
class QScrollArea;
class QScrollBar;
class SImageViewer : public QMainWindow
{
    Q_OBJECT
public:
    explicit SImageViewer(const QString& imagePath, QWidget* parent = nullptr);

private slots:
    void toggleFullScreen();
    void zoomIn();
    void zoomOut();
    void normalSize();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
private:
    void createActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar* scrollBar, double factor); 
    void fitToWindow();

    bool fitToWindowEnabled;
    QScrollArea* scrollArea{};
    QLabel* imageLabel{};
    double scaleFactor = 1.0;
};

#endif // SIMAGEVIEWER_H