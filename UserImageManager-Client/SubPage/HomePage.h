#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

class SImageShowWidget;
class QStackedWidget;
class HomePage : public QWidget
{
	Q_OBJECT
public:
	HomePage(QWidget* parent = nullptr);
	~HomePage();
	void init();
private:
	
};

#endif // HOMEPAGE_H