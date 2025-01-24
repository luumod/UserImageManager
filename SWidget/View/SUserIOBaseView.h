#ifndef SIODIALOG_H_
#define SIODIALOG_H_

#include <QWidget>
#include <QPushButton>
#include <QLabel>
class SUserIOBaseView : public QWidget
{
	Q_OBJECT
public:
	SUserIOBaseView(QWidget* parent = nullptr);

	void setTitle(const QString& title);
	QString title()const;

	QWidget* centralWidget()const;
private:
	void init();

	//标题栏
	QLabel* m_titleLab{};
	QPushButton* m_closeBtn{};
	//中心窗口
	QWidget* m_centraWidget{};
};

#endif // !SIODIALOG_H_
