#ifndef SIMAGESJOINTWIDGET_H_
#define SIMAGESJOINTWIDGET_H_

#include <QWidget>

class SImagesJointWidget : public QWidget {
public:
    SImagesJointWidget(QWidget* parent = nullptr);
    ~SImagesJointWidget();

    void init();

protected:
    void paintEvent(QPaintEvent* event)override;
private:

};

#endif //!SIMAGESJOINTWIDGET_H_