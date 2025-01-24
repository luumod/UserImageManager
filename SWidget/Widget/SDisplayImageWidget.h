#ifndef SDISPLAYIMAGEWIDGET_H
#define SDISPLAYIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

class QLabel;
class SDisplayImageWidget : public QWidget
{
	Q_OBJECT

public:
	SDisplayImageWidget(int image_id, const QString& imagePath, QWidget* parent = nullptr);
	SDisplayImageWidget(int id, QWidget* parent = nullptr);
	SDisplayImageWidget(QWidget* parent = nullptr);
	~SDisplayImageWidget();

	void init();
	bool isNull()const;
	void setImagePath(const QString& filePath);
signals:
	void clickedImage(int id);
protected:
	void mousePressEvent(QMouseEvent* event)override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	QLabel* m_pixmapLab{};
	QString m_imagePath{};
	int m_image_id{}; //must：用于跳转到该图片
};

#endif // SDISPLAYIMAGEWIDGET_H