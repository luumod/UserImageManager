#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QFile>
#include <QFileInfo>
#include "SImageInfo.h"

class SUploadSingleImageView;
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
private:

	SUploadSingleImageView* m_uploadSingle{};
};

#endif // HOMEPAGE_H