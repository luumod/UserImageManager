#ifndef IMAGEDETAILPAGE_H
#define IMAGEDETAILPAGE_H

#include <QWidget>
#include "SImageInfo.h"

class SImageInfoWidget;
class SUserCommentView;
class SImageDetailView :public QWidget
{
	Q_OBJECT

public:
	enum class ImageDetailPageType
	{
		In_SPersonalImagePage,
		In_SImageManagerPage,
		In_SHomePage
	};
	SImageDetailView(ImageDetailPageType pageType, QWidget* parent = nullptr);
	~SImageDetailView();

	void init();
	void setData(ImageInfo info, int id);
	void setData(ImageInfo info);
signals:
	void nextImage(int nextIndex);
	void prevImage(int prevIndex);
	void imageLiked(int image_index);
	void imageUnLiked(int image_index);
	void imageStared(int image_index);
	void imageUnStared(int image_index);
	void imageDownloaded(int image_index);
	void commentCountChange(int image_index, int count);
public:
	ImageDetailPageType m_pageType;

	SImageInfoWidget* m_imageDetailDlg{};
	SUserCommentView* m_commentDlg{};
};


#endif // IMAGEDETAILPAGE_H