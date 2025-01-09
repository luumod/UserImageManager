#ifndef SIMAGESHOWWIDGET_H
#define SIMAGESHOWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>

class RoundedImageWidget;
class SImageShowWidget : public QWidget
{
	Q_OBJECT
public:
	SImageShowWidget(int w,int h,const QString& path, const QString& name, const QString& desc, QWidget* parent = nullptr);
	SImageShowWidget(QWidget* parent = nullptr);
	~SImageShowWidget();
	void init();
	void loadImage(const QString& path, const QString& name, const QString& desc);
	void paintEvent(QPaintEvent* event)override;
private:
	RoundedImageWidget* m_wid_labImage{};
	QLabel* m_lab_imageName{};
	QTextEdit* m_area_textArea{};

	QString m_imagePath;
	QString m_imageName;
	QString m_imageDesc;
	int m_w;
	int m_h;
};  

#endif // SIMAGESHOWWIDGET_H