#ifndef SDISPLAYIMAGEWIDGET_H
#define SDISPLAYIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include "SImageInfo.h"

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
	void setData(const QString& path, int heat);
signals:
	void doubleClickedToOpenImageDetail(const QString& path);
protected:
	void mousePressEvent(QMouseEvent* event)override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	QLabel* m_pixmapLab{};
	QLabel* m_heatLab{};

	QString m_imagePath{};
};

#endif // SDISPLAYIMAGEWIDGET_H