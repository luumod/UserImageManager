#ifndef EVENTFILTEROBJECT_H
#define EVENTFILTEROBJECT_H

#include <QObject>
#include <QPoint>

class EventFilterObject :public QObject {
public:
	EventFilterObject(QObject* parent = nullptr);
	~EventFilterObject();

protected:
	bool eventFilter(QObject* watched, QEvent* event)override;

	QPoint m_pressPos;
};


#endif // EVENTFILTEROBJECT_H