#ifndef MAINWINDOW_H_
#define  MAINWINDOW_H_

#include "UserManagerPage.h"
#include "HomePage.h"
#include "PersonalInfo.h"
#include "STopBar.h"
#include <QWidget>
#include <QStackedWidget>
#include <QTreeWidget>


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
	PersonalInfo* m_personalInfoPage{};
};

#endif // !MAINWINDOW_H_
