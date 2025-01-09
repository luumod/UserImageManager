#ifndef STopBar_H
#define STopBar_H

#include <QWidget>
#include <QLabel>

class STopBar : public QWidget
{
public:
    STopBar(QWidget*parent = nullptr);
    ~STopBar();
    void init();
private:
    QLabel* title;

    void initTitle();
};

#endif // STopBar_H