#include "SButtonDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QAbstractItemView>

#define BUTTON_WIDTH(id) m_buttons.at(id)->width()
#define BUTTON_HEIGHT(id) m_buttons.at(id)->height()

SButtonDelegate::SButtonDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

QAbstractButton* SButtonDelegate::addButton(QAbstractButton* button)
{
	m_buttons.emplace_back(button);
	return button;
}

QAbstractButton* SButtonDelegate::button(int id)
{
	Q_ASSERT_X(id >= 0 && id < m_buttons.size(), "SButtonDelegate::button", "Invalid button id");
	return m_buttons.at(id).get();
}

int SButtonDelegate::buttonTotalWidth() const
{
	int w = 0;
	for (const auto& x : m_buttons) {
		w += x->width(); //45
	}
	return w;
}

void SButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();

	auto totalWidth = buttonTotalWidth();
	int hspace = (option.rect.width() - (totalWidth + m_spacing * (m_buttons.size() - 1))) / 2;
	int vspace = (option.rect.height() - BUTTON_HEIGHT(0)) / 2;

	for (int i = 0; i < m_buttons.size(); i++) {
		QStyleOptionButton button;
		button.rect = QRect(option.rect.x() + hspace + i * (BUTTON_WIDTH(i) + m_spacing),
			option.rect.y() + vspace, BUTTON_WIDTH(i), BUTTON_HEIGHT(i));
		button.state |= QStyle::State_Enabled; 

		if (option.state & QStyle::State_MouseOver && button.rect.contains(m_mousePos)) {
			button.state |= QStyle::State_MouseOver;
		}

		auto btn = m_buttons.at(i).get();
		button.text = btn->text();
		//btn->style()->drawControl(QStyle::CE_PushButton, &button,painter,btn);

		 // 手动绘制按钮
		painter->setRenderHint(QPainter::Antialiasing); // 启用抗锯齿
		painter->fillRect(button.rect, QColor("lightblue")); // 设置背景颜色
		painter->setPen(QPen(QColor("grey"), 1)); // 设置边框颜色
		painter->drawRoundedRect(button.rect, 5, 5); // 绘制圆角矩形

		// 绘制按钮文本
		painter->setPen(QColor("black")); // 设置文本颜色
		
		// 如果鼠标悬停，改变背景颜色
		if (button.state & QStyle::State_MouseOver) {
			painter->fillRect(button.rect, QColor("#83CFF5"));
			painter->setPen(QColor("black")); // 设置文本颜色
			painter->drawText(button.rect, Qt::AlignCenter, button.text);
		}
		else {
			painter->drawText(button.rect, Qt::AlignCenter, button.text);
		}


	}
	painter->restore();
}

bool SButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseMove) {
		m_mousePos = dynamic_cast<QMouseEvent*>(event)->position().toPoint();

		auto view = dynamic_cast<QAbstractItemView*>(parent());
		if (view) {
			view->viewport()->update(option.rect);
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) {

		auto totalWidth = buttonTotalWidth();
		int hspace = (option.rect.width() - (totalWidth + m_spacing * (m_buttons.size() - 1))) / 2;
		int vspace = (option.rect.height() - BUTTON_HEIGHT(0)) / 2;

		for (int i = 0; i < m_buttons.size(); i++) {
			QRect rect(option.rect.x() + hspace + i * (BUTTON_WIDTH(i) + m_spacing),
				option.rect.y() + vspace, BUTTON_WIDTH(i), BUTTON_HEIGHT(i));
			if (rect.contains(m_mousePos)) {
				emit buttonClicked(i,index);
			}
		}
	}
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
