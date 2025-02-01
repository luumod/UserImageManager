#ifndef SRESCENTMOVEMENTVIEW_H_
#define SRESCENTMOVEMENTVIEW_H_

#include <QWidget>

class QVBoxLayout;
class SUserMessageWidget;
class SRescentMovementWidget : public QWidget
{
    Q_OBJECT

public:
    SRescentMovementWidget(QWidget* parent = 0);
    ~SRescentMovementWidget();

    void initWidgets();
    void init();

    void onSearch();
    void parseJson(const QJsonObject& data);

    void setData(int idx, const QString& avator_path, const QString& user_name, const QString& message, const QString& dateTime);
    void clearData();
protected:
    void paintEvent(QPaintEvent* event)override;
private:
    QVBoxLayout* m_contentLayout{};
    QList<SUserMessageWidget*> m_messageWidgets;
    int m_currentPage = 1;
    int m_lastPage{};
    int m_totalCount{};
    static constexpr int MAX_ACTIVITIES_IN_ONE_PAGE = 7;
};

#endif // !
