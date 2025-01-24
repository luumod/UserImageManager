#include "SImageDelegate.h"
#include "SImage.h"
#include "STimer.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QAbstractItemView>

SImageDelegate::SImageDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

void SImageDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{//paint在主线程中执行，不能进行异步操作
	painter->save();
	auto path = QDir::currentPath() + "/" + index.data(Qt::UserRole).toString();

	QPixmap scaledPixmap = SImage::loadAndCropImage(path, option.rect);
	painter->drawPixmap(option.rect.x() + (option.rect.width() - scaledPixmap.width()) / 2, option.rect.y() + (option.rect.height() - scaledPixmap.height()) / 2, scaledPixmap);
	painter->restore();
}


bool SImageDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
