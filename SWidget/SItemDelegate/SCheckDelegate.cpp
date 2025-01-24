#include "SCheckDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>

SCheckDelegate::SCheckDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{

}

void SCheckDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//选中还是未选中还是半选中
	bool checkState = index.data(Qt::UserRole).toBool();

	QStyleOptionButton checkBoxStyle;
	QCheckBox cbx;
	checkBoxStyle.initFrom(&cbx);
	checkBoxStyle.rect = option.rect;
	checkBoxStyle.state = checkState? QStyle::State_On : QStyle::State_Off;
	checkBoxStyle.state |= QStyle::State_Enabled;

	QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter, nullptr);

}

bool SCheckDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	auto ev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress && 
		option.rect.contains(ev->position().toPoint())) {
		bool state = !index.data(Qt::UserRole).toBool();
		model->setData(index, state, Qt::UserRole);
		if (state) {
			m_checkCount++;
		}
		else {
			m_checkCount--;
		}
		emit stateChanged(!state,index);
	}
	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
