#ifndef IMAGEDETAILPAGE_H
#define IMAGEDETAILPAGE_H

#include <QWidget>
#include "SImageInfo.h"

class SImageDetailDlg;
class SCommentDlg;
class ImageDetailPage :public QWidget
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
	SCommentDlg* m_commentDlg{};
};


#endif // IMAGEDETAILPAGE_H