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
	void imageLiked(int image_index);
	void imageUnLiked(int image_index);
	void imageStared(int image_index);
	void imageUnStared(int image_index);
	void imageDownloaded(int image_index);
public:
	SImageDetailDlg* m_imageDetailDlg{};
};


#endif // IMAGEDETAILPAGE_H