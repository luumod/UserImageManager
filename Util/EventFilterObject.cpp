#include "EventFilterObject.h"
#include <QMouseEvent>
#include <QWidget>
#include <QGraphicsDropShadowEffect> //阴影效果

EventFilterObject::EventFilterObject(QObject* parent)
{
	auto w = dynamic_cast<QWidget*>(parent);
	if (w) {
		//设置阴影效果
		auto shadow = new QGraphicsDropShadowEffect(this);
		shadow->setOffset(0);
		shadow->setBlurRadius(10);
		shadow->setColor(QColor(60, 60, 60));
		w->setGraphicsEffect(shadow);
	}
}

EventFilterObject::~EventFilterObject()
{

}

bool EventFilterObject::eventFilter(QObject* watched, QEvent* event)
{
	QMouseEvent* mev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress) {
		m_pressPos = mev->pos();
	}
	else if (event->type() == QEvent::MouseMove && mev->buttons() & Qt::LeftButton) {
		QWidget* w = dynamic_cast<QWidget*>(watched);
		if (w) {
			w->move(mev->globalPos() - m_pressPos);
		}
	}
	return false;
}
