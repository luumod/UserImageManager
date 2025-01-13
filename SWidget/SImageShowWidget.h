#ifndef SIMAGESHOWWIDGET_H
#define SIMAGESHOWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPaintEvent>

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

	inline RoundedImageWidget* getImageShowWidget()const { return m_wid_labImage; }
	inline int getId()const { return m_id; }
protected:
	void paintEvent(QPaintEvent* event)override;

private:
	RoundedImageWidget* m_wid_labImage{};


	QLabel* m_lab_imageName{};
	QLabel* m_area_textArea{};

	QString m_imagePath;
	QString m_imageName;
	QString m_imageDesc;

	int m_id{};
};  

#endif // SIMAGESHOWWIDGET_H