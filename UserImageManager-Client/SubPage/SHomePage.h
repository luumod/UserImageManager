#ifndef SHOMEPAGE_H
#define SHOMEPAGE_H

#include <QWidget>

class SDisplayImagesScrollAreaWidget;
class SImagesJointWidget;
class SRescentMovementView;
class SHomePage : public QWidget
{
public:   
    SHomePage(QWidget* parent = nullptr);
    ~SHomePage();

    void init();

protected:

private:
    SDisplayImagesScrollAreaWidget* m_displayWidget{};
    SImagesJointWidget* m_jointWidget{};
    SRescentMovementView* m_recentMovementView{};
};


#endif // SHOMEPAGE_H