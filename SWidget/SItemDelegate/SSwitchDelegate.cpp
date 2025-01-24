#include "SSwitchDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QAbstractItemView>

#define BUTTON_WIDTH 40
#define BUTTON_HEIGHT 17

SSwitchDelegate::SSwitchDelegate(UseInWhat use,QObject* parent)
	: QStyledItemDelegate(parent)
	, m_useInWhat(use)
{

}

void SSwitchDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	auto state = index.data(Qt::UserRole).toBool();
	QString text = "";
	if (m_useInWhat == UseInWhat::UserManager) {
		 text = state? "启用" : "禁用";
	}
	else if (m_useInWhat == UseInWhat::ImageManager) {
		text = state? "删除" : "正常";
	}
	painter->setRenderHint(QPainter::Antialiasing);

	//文本大小
	int tw = painter->fontMetrics().horizontalAdvance(text);
	int th = painter->fontMetrics().height();
	int hspace = (option.rect.width()-(tw + BUTTON_WIDTH)) / 2;
	int vspace = (option.rect.height() - th) / 2;
	//绘制文本
	painter->drawText(QPoint(option.rect.x() + hspace, option.rect.bottom() - vspace), text);
	
	//绘制按钮
	if (m_useInWhat == UseInWhat::UserManager) {
		painter->setBrush(state ? QColor(97, 162, 243) : QColor(136, 136, 138));
	}
	else if (m_useInWhat == UseInWhat::ImageManager) {
		painter->setBrush(state ? QColor(136, 136, 138) : QColor(97, 162, 243));
	}
	painter->setPen(Qt::gray);
	QRect btnRect = { option.rect.x() + hspace + tw + 5, option.rect.y() + (option.rect.height() - BUTTON_HEIGHT)/2, BUTTON_WIDTH, BUTTON_HEIGHT};
	painter->drawRoundedRect(btnRect, btnRect.height() / 2, btnRect.height() / 2);

	//小球
	//在单元格内，并且仅仅在小球的范围内鼠标放置上面会变色
	if (option.state & QStyle::State_MouseOver && btnRect.contains(m_mousePos)) {  
		painter->setBrush(QColor(232, 232, 232));
	}
	else {
		painter->setBrush(Qt::white);
	}
	if (m_useInWhat == UseInWhat::UserManager) {
		if (state) {
			painter->drawEllipse(btnRect.right() - BUTTON_HEIGHT, btnRect.y(), BUTTON_HEIGHT, BUTTON_HEIGHT);
		}
		else {
			painter->drawEllipse(btnRect.x(), btnRect.y(), BUTTON_HEIGHT, BUTTON_HEIGHT);
		}
	}
	else if (m_useInWhat == UseInWhat::ImageManager) {
		if (state) {
			painter->drawEllipse(btnRect.x(), btnRect.y(), BUTTON_HEIGHT, BUTTON_HEIGHT);
		}
		else {
			painter->drawEllipse(btnRect.right() - BUTTON_HEIGHT, btnRect.y(), BUTTON_HEIGHT, BUTTON_HEIGHT);
		}
	}
	painter->restore();
}

bool SSwitchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseButtonRelease) {
		auto state  = model->data(index, Qt::UserRole).toBool();
		model->setData(index, !state, Qt::UserRole); //切换状态
		emit stateChanged(!state, index);
	}
	else if (event->type() == QEvent::MouseMove) {
		m_mousePos = dynamic_cast<QMouseEvent*>(event)->position().toPoint();

		auto view = dynamic_cast<QAbstractItemView*>(parent());
		if (view) {
			view->viewport()->update(option.rect); //更新，调用paint
		}
	}
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
