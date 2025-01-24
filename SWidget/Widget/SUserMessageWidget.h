#ifndef SUSERMESSAGEWIDGET_H
#define SUSERMESSAGEWIDGET_H

#include <QWidget>

class SUserMessageWidget : public QWidget
{
    Q_OBJECT
public:
    SUserMessageWidget(int image_id, QString user_name, QString message, QString dateTime, QWidget *parent = nullptr);
    SUserMessageWidget(QWidget *parent = nullptr);
    ~SUserMessageWidget();

    void init();

private:
    int m_image_id{};
    QString m_user_name{};
    QString m_message{};
    QString m_dateTime{};
};

#endif // SUSERMESSAGEWIDGET_H