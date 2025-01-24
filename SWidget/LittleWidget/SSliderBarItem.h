#ifndef SSLIDERBARITEM_H_
#define SSLIDERBARITEM_H_

#include <QWidget>

class QLabel;
class SSliderBarItem :public QWidget {
	Q_OBJECT
public:
	SSliderBarItem(int id,const QString& iconPath,const QString& title,QWidget* parent = nullptr);
	~SSliderBarItem();

	void init();
signals:
	void clickedOnePage(int id);
protected:
	void enterEvent(QEnterEvent* event)override;
	void leaveEvent(QEvent* event)override;
	void mousePressEvent(QMouseEvent* event)override;
private:
	int m_id;
	QString m_iconPath;
	QString m_title;
	QLabel* m_labIcon{};
	QLabel* m_labTitle{};
};

#endif