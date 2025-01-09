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
	void onSearch();
	void parseJson(const QJsonObject& json);
	void test_images_4();
signals:
	void parseFinised();
private:
	QStackedWidget* m_stkedWidget;
	QList<SImageShowWidget*> m_images;
};

#endif // HOMEPAGE_H