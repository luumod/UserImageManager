#ifndef SSLIDERBAR_H_
#define SSLIDERBAR_H_

#include <QWidget>

class SSliderBarItem;
class SSliderBar :public QWidget {
	Q_OBJECT

public:
	SSliderBar(QWidget* parent = nullptr);
	~SSliderBar();
	
	void init();
signals:
	void clickedOneItem(int id);
private:
	QList<SSliderBarItem*> m_sliderBarItems;

};

#endif