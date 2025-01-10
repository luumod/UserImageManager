#ifndef MAINWINDOW_H_
#define  MAINWINDOW_H_


#include <QWidget>


class UserManagerPage;
class HomePage;
class PersonalSpace;
class STopBar;
class PersonalDetail;
class QTreeWidget;
class QStackedWidget;
class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	void init();

private:
	QWidget* m_topBar{};
	QTreeWidget* m_treeNavBar{};
	QStackedWidget* m_stkWidget{};
	//初始化顶部栏
	QWidget* initTopBar();
	//初始化内容框
	QWidget* initConetentArea();

	UserManagerPage* m_userManagerPage{};
	HomePage* m_homePage{};
	PersonalSpace* m_personalInfoPage{};
	PersonalDetail* m_personalDetailPage{};
};

#endif // !MAINWINDOW_H_
