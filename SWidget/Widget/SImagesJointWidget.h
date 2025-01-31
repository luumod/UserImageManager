#ifndef SIMAGESJOINTWIDGET_H_
#define SIMAGESJOINTWIDGET_H_

#include <QWidget>
#include <QLabel>

class SImageJointShowWidget: public QLabel {
	Q_OBJECT
public:
	SImageJointShowWidget(const QString& path, QWidget* parent = nullptr);
	~SImageJointShowWidget();

	void setImagePath(const QString& path);
protected:
	void mouseReleaseEvent(QMouseEvent* event)override;
signals:
	void openImageViewer(const QString& path);
private:
	QString m_image_path;
};

class QGridLayout;
class SImagesJointWidget : public QWidget {
	Q_OBJECT
public:
	SImagesJointWidget(QWidget* parent = nullptr);
	~SImagesJointWidget();

	void init();
	void setData(const QJsonArray& Nhot_images_all);
protected:
	void paintEvent(QPaintEvent* event)override;
	void resizeEvent(QResizeEvent* event)override;
signals:
	void notifyNHotsImagesLoaded();
	void showImageViewer(const QString& path);
private:
	QGridLayout* grid_layout{};
	QStringList imagePaths;
	QList<SImageJointShowWidget*> m_image_labels{};

	static constexpr int MAX_IMAGE_SIZE = 7;
};

#endif //!SIMAGESJOINTWIDGET_H_