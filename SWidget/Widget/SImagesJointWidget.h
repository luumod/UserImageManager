#ifndef SIMAGESJOINTWIDGET_H_
#define SIMAGESJOINTWIDGET_H_

#include <QWidget>

class QGridLayout;
class QLabel;
class SImagesJointWidget : public QWidget {
public:
    SImagesJointWidget(QWidget* parent = nullptr);
    ~SImagesJointWidget();

    void init();

protected:
    void paintEvent(QPaintEvent* event)override;
    void resizeEvent(QResizeEvent* event)override;
private:
    QGridLayout* grid_layout{};

    QList<QLabel*> m_image_labels{};
};

#endif //!SIMAGESJOINTWIDGET_H_