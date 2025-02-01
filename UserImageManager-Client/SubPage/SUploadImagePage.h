#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include "SImageInfo.h"

class SUploadSingleImageView;
class SUploadMultiImageView;
class SUploadImagePage : public QWidget
{
	Q_OBJECT
public:
	SUploadImagePage(QWidget* parent = nullptr);
	~SUploadImagePage();

	void init();

protected:
	void resizeEvent(QResizeEvent* event) override;
public slots:
	void onUploadSingleImageClicked();
	void onUploadSomeImageClicked();
private:

	SUploadSingleImageView* m_uploadSingle{};
	SUploadMultiImageView* m_uploadMulti{};
};

#endif // HOMEPAGE_H