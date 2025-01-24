#include "SImageDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QAbstractItemView>

#define IMAGE_WIDTH 100
#define IMAGE_HEIGHT 100

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

	painter->drawPixmap(option.rect.x() + (option.rect.width() - IMAGE_WIDTH) / 2, option.rect.y() + (option.rect.height() - IMAGE_HEIGHT) / 2, IMAGE_WIDTH, IMAGE_HEIGHT, pixmap);

	painter->restore();
}

bool SImageDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
