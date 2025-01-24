#ifndef MAINWINDOW_H_
#define  MAINWINDOW_H_


#include <QWidget>


class SHomePage;
class SUserManagerPage;
class SUploadImagePage;
class SPersonalImagePage;
class SPersonalCenterPage;
class SImageManagerPage;
class QStackedWidget;
class SSliderBar;
class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void init();

private:
	SSliderBar* m_sliderBar{};
	QStackedWidget* m_stkWidget{};
	SHomePage* m_homePage{};
	SUserManagerPage* m_userManagerPage{};
	SUploadImagePage* m_uploadImagePage{};
	SImageManagerPage* m_imageManagerPage{};
	SPersonalImagePage* m_personalImagePage{};
	SPersonalCenterPage* m_personalDetailPage{};
};

#endif // !MAINWINDOW_H_
