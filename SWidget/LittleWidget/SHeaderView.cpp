#include "SHeaderView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCheckBox>
#define CHECK_BOX_COLUMN 0

SHeaderView::SHeaderView(Qt::Orientation orientation, QWidget *parent)
	: QHeaderView(orientation, parent),
	  m_bPressed(false),
	  m_bChecked(false),
	  m_bTristate(false),
	  m_bNoChange(false),
	  m_bHover(false)
{
	// setStretchLastSection(true);
	setHighlightSections(false);
	setMouseTracking(true);

	// 响应鼠标
	setSectionsClickable(true);
}

// 槽函数，用于更新复选框状态
void SHeaderView::setState(int state)
{
	if (state == Qt::PartiallyChecked) {
		m_bTristate = true;
		m_bNoChange = true;
	} else {
		m_bNoChange = false;
	}

	m_bChecked = (state != Qt::Unchecked);
	viewport()->update();
}

// 绘制复选框
void SHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
	painter->save();
	QHeaderView::paintSection(painter, rect, logicalIndex);
	painter->restore();

	if (logicalIndex == CHECK_BOX_COLUMN)
	{
		QStyleOptionButton option;
		QCheckBox checkBox;
		option.initFrom(&checkBox);

		if (m_bChecked)
			option.state |= QStyle::State_Sunken;

		if (m_bTristate && m_bNoChange)
			option.state |= QStyle::State_NoChange;
		else
			option.state |= m_bChecked ? QStyle::State_On : QStyle::State_Off;
		if (testAttribute(Qt::WA_Hover) && underMouse()) {
			if (m_bHover)
				option.state |= QStyle::State_MouseOver;
			else
				option.state &= ~QStyle::State_MouseOver;
		}

		option.iconSize = QSize(20, 20);
		option.rect = rect;
		//option.rect = QRect(rect.x() + 3, rect.y() + (rect.height() - 16)/2, 14, 14);
		style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
		//style()->drawItemPixmap(painter, rect, Qt::AlignCenter, QPixmap(":/images/checkBoxChecked"));
		//style()->drawControl(QStyle::CE_CheckBox, &option, painter, this);
	}
}

// 鼠标按下表头
void SHeaderView::mousePressEvent(QMouseEvent *event)
{
	int nColumn = logicalIndexAt(event->pos());
	if ((event->buttons() & Qt::LeftButton) && (nColumn == CHECK_BOX_COLUMN))
	{
		m_bPressed = true;
	}
	else
	{
		QHeaderView::mousePressEvent(event);
	}
}

// 鼠标从表头释放，发送信号，更新model数据
void SHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_bPressed)
	{
		if (m_bTristate && m_bNoChange)
		{
			m_bChecked = true;
			m_bNoChange = false;
		}
		else
		{
			m_bChecked = !m_bChecked;
		}

		viewport()->update();

		Qt::CheckState state = m_bChecked ? Qt::Checked : Qt::Unchecked;

		emit stateChanged(state);
	}
	else
	{
		QHeaderView::mouseReleaseEvent(event);
	}

	m_bPressed = false;
}

// 鼠标滑过、离开，更新复选框状态
bool SHeaderView::event(QEvent *event)
{
	updateSection(0);
	if (event->type() == QEvent::Enter)
	{
		QEnterEvent* pEvent = dynamic_cast<QEnterEvent*>(event);
		int nColumn = logicalIndexAt(pEvent->position().toPoint());
		if (nColumn == CHECK_BOX_COLUMN)
		{
			m_bHover = true;

			return true;
		}
	}
	else if (event->type() == QEvent::Leave)
	{
		m_bHover = false;
		return true;
	}

	return QHeaderView::event(event);
}


