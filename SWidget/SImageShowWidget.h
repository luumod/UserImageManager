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
	SImageShowWidget(int id,const QString& path, const QString& name, const QString& desc, QWidget* parent = nullptr);
	SImageShowWidget(int id,QWidget* parent = nullptr);
	~SImageShowWidget();
	void init();
	void loadImage(const QString& path, const QString& name, const QString& desc);

	void paintEvent(QPaintEvent* event)override;

	RoundedImageWidget* m_wid_labImage{};
private:


	QLabel* m_lab_imageName{};
	QTextEdit* m_area_textArea{};

	QString m_imagePath;
	QString m_imageName;
	QString m_imageDesc;

	int m_id{};
};  

#endif // SIMAGESHOWWIDGET_H