#ifndef ROUNDEDIMAGEWIDGET_H
#define ROUNDEDIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

class QLabel;
class RoundedImageWidget : public QWidget
{
	Q_OBJECT

public:
	RoundedImageWidget(int id, const QString& filePath, QWidget* parent = nullptr);
	RoundedImageWidget(int id, QWidget* parent = nullptr);
	RoundedImageWidget(QWidget* parent = nullptr);
	~RoundedImageWidget();

	void init();
	void setStyle();
	bool isNull()const;
	void setImagePath(const QString& filePath);
signals:
	void clickedImage(int id);
protected:
	void mouseMoveEvent(QMouseEvent* event)override;
	void mousePressEvent(QMouseEvent* event)override;

private:
	QLabel* m_pixmapLab{};
	QString m_filePath{};
	int m_id{};
};

#endif // ROUNDEDIMAGEWIDGET_H