#ifndef SRESCENTMOVEMENTVIEW_H_
#define SRESCENTMOVEMENTVIEW_H_

#include <QWidget>

class QVBoxLayout;
class SRescentMovementView : public QWidget
{
    Q_OBJECT

public:
    SRescentMovementView(QWidget* parent = 0);
    ~SRescentMovementView();

    void init();

    void addMessage(int image_id, QString user_name, QString message, QString dateTime);
protected:
    void paintEvent(QPaintEvent* event)override;
private:
    //SUserMessageWidget* m_pUserMessageWidget{};
    QVBoxLayout* m_contentLayout{};
};

#endif // !
