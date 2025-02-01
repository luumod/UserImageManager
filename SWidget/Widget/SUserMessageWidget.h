#ifndef SUSERMESSAGEWIDGET_H
#define SUSERMESSAGEWIDGET_H

#include <QWidget>

class QLabel;
class SUserMessageWidget : public QWidget
{
    Q_OBJECT
public:
    SUserMessageWidget(QWidget* parent=nullptr);
    SUserMessageWidget(const QString& avatar_path, const QString& user_name, const QString& message, const QString& dateTime, QWidget *parent = nullptr);
    ~SUserMessageWidget();

    void init();
    void updateUi();
    void setData(const QString& avatar_path, const QString& user_name, const QString& message, const QString& dateTime);
    void clearData();
private:
    QString m_avatar_path{};
    QString m_user_name{};
    QString m_message{};
    QString m_dateTime{};

    QLabel* m_image_label{};
    QLabel* m_user_name_label{};
    QLabel* m_message_label{};
    QLabel* m_date_time_label{};
};

#endif // SUSERMESSAGEWIDGET_H