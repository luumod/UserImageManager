#ifndef S_PAGE_TOP_WIDGET_H
#define S_PAGE_TOP_WIDGET_H

#include <QWidget>

class SPageTopWidget : public QWidget
{
    Q_OBJECT
public:
    SPageTopWidget(QWidget *parent = nullptr);
    ~SPageTopWidget();

    void init();
private:
};  

#endif // S_PAGE_TOP_WIDGET_H