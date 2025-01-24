#ifndef SSLIDERBARBOTTOMINFOITEM_H
#define SSLIDERBARBOTTOMINFOITEM_H

#include <QWidget>

class SSliderBarBottomInfoItem : public QWidget
{
public:
    SSliderBarBottomInfoItem(const QString& name, const QString& desc, const QString& avatarPath, QWidget *parent = nullptr);
    ~SSliderBarBottomInfoItem();

    void init();
private:
    QString m_name;
    QString m_desc;
    QString m_avatarPath;
};


#endif // SSLIDERBARBOTTOMINFOITEM_H