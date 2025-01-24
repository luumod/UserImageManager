#ifndef ROUNDEDIMAGEWIDGET_H
#define ROUNDEDIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

class QLabel;
class SRoundedImageWidget : public QWidget
{
	Q_OBJECT

public:
	SRoundedImageWidget(int id, const QString& filePath, QWidget* parent = nullptr);
	SRoundedImageWidget(int id, QWidget* parent = nullptr);
	SRoundedImageWidget(QWidget* parent = nullptr);
	~SRoundedImageWidget();

	void init();
	bool isNull()const;
	void setImagePath(const QString& filePath);

signals:
	void clickedImage(int id);
protected:
	void mouseMoveEvent(QMouseEvent* event)override;
	void leaveEvent(QEvent* event)override;
	void mousePressEvent(QMouseEvent* event)override;

private:
	QLabel* m_pixmapLab{};
	QString m_filePath{};
	int m_id{};
};

#endif // ROUNDEDIMAGEWIDGET_H