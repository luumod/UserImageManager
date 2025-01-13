#ifndef IMAGEDETAILPAGE_H
#define IMAGEDETAILPAGE_H

#include <QScrollArea>
#include "SImageInfo.h"

class SImageDetailDlg;
class ImageDetailPage :public QScrollArea
{
	Q_OBJECT
public:
	ImageDetailPage(QWidget* parent = nullptr);
	~ImageDetailPage();

	void init();
	void setData(ImageInfo info, int id);
signals:
	void nextImage(int nextIndex);
	void prevImage(int prevIndex);
private:
	int m_currentImageIndex{};
	SImageDetailDlg* m_imageDetailDlg{};
};


#endif // IMAGEDETAILPAGE_H