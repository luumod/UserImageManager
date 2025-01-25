#include "SImage.h"
#include <iostream>
#include <QRect>
#include <QPixmap>
#include <QLabel>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

SImage::SImage()
{
}

SImage::~SImage()
{
}

void SImage::loadAndCropImage(const QString& filePath, QLabel* pixmapLab, LoadInWhere loadTpye)
{
	if (loadTpye == LoadInWhere::Other) {
		pixmapLab->clear();
	}
	QFuture<QPixmap> future = QtConcurrent::run([filePath, pixmapLab]() {
		QPixmap pixmap(filePath);
		QRect cropRect = cropPixmap(pixmap, pixmapLab->rect());
		if (cropRect == QRect(QPoint(0, 0), pixmap.size())) {
			return pixmap;
		}
		else {
			return pixmap.copy(cropRect);
		}
		});

	// 创建一个QFutureWatcher来监听异步操作的结果
	QFutureWatcher<QPixmap>* watcher = new QFutureWatcher<QPixmap>(pixmapLab);
	connect(watcher, &QFutureWatcher<QPixmap>::finished, pixmapLab, [pixmapLab, watcher]() {
		QPixmap croppedPixmap = watcher->result();
		pixmapLab->setPixmap(croppedPixmap.scaled(pixmapLab->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		watcher->deleteLater();
		});

	watcher->setFuture(future);
}

const QPixmap SImage::loadAndCropImage(const QString& filePath, const QRect& previewRect)
{
	QPixmap pixmap(filePath);
	QRect cropRect = cropPixmap(pixmap, previewRect);
	if (cropRect == QRect(QPoint(0, 0), pixmap.size())) {
		return pixmap.scaled(previewRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	else {
		return pixmap.copy(cropRect).scaled(previewRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

}

const QRect SImage::cropPixmap(const QPixmap& pixmap,const QRect& previewRect)
{
	int originalWidth = pixmap.width();
	int originalHeight = pixmap.height();
	int previewWidth = previewRect.width();
	int previewHeight = previewRect.height();

	// 计算宽高比
	float aspectRatioPreview = (float)previewWidth / previewHeight;
	float aspectRatioImage = (float)originalWidth / originalHeight;

	// 确定裁剪区域的宽度和高度
	int cropWidth, cropHeight;

	if (aspectRatioPreview > aspectRatioImage) {
		//预览区域以宽为准，裁剪区域的宽度等于原图宽度
		cropWidth = originalWidth;
		cropHeight = cropWidth / aspectRatioPreview;  // 根据宽高比计算高度
	}
	else {
		//预览区域以高为准，裁剪区域的高度等于原图高度
		cropHeight = originalHeight;
		cropWidth = cropHeight * aspectRatioPreview;  // 根据宽高比计算宽度
	}

	// 计算裁剪区域的起始位置
	int centerX = (originalWidth - cropWidth) / 2;
	int centerY = (originalHeight - cropHeight) / 2;

	// 确保裁剪区域在图片内部
	centerX = std::max(0, centerX);
	centerY = std::max(0, centerY);
	cropWidth = std::min(cropWidth, originalWidth - centerX);
	cropHeight = std::min(cropHeight, originalHeight - centerY);

	return QRect(centerX, centerY, cropWidth, cropHeight);
}
