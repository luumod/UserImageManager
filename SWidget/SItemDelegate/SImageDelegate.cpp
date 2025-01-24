#include "SImageDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QAbstractItemView>

SImageDelegate::SImageDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

void SImageDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	auto relPath = index.data(Qt::UserRole).toString();
	auto pixmap = QPixmap(QDir::currentPath() + "/" + relPath);
	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	QPixmap scaledPixmap = pixmap.scaled(option.rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	painter->drawPixmap(option.rect.x() + (option.rect.width() - scaledPixmap.width()) / 2, option.rect.y() + (option.rect.height() - scaledPixmap.height()) / 2, scaledPixmap);
	painter->restore();
}

bool SImageDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
