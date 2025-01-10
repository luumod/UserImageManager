#ifndef ROUNDEDIMAGEWIDGET_H
#define ROUNDEDIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

class RoundedImageWidget : public QWidget
{
	Q_OBJECT

public:
	RoundedImageWidget(int id, const QPixmap& pixmap, QWidget* parent = nullptr);
	RoundedImageWidget(int id, QWidget* parent = nullptr);
	RoundedImageWidget(QWidget* parent = nullptr);
	~RoundedImageWidget();
	void setPixmap(const QString& path);
	bool isNull()const;
signals:
	void clickedImage(int id);
protected:
	void mouseMoveEvent(QMouseEvent* event)override;
	void mousePressEvent(QMouseEvent* event)override;
	void paintEvent(QPaintEvent* event)override;


private:
	QPixmap m_pixmap;

	int m_id{};
};

#endif // ROUNDEDIMAGEWIDGET_H