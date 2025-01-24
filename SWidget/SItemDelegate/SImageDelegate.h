#ifndef SIMAGEDELEGATE_H
#define SIMAGEDELEGATE_H

#include <QStyledItemDelegate>

class SImageDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SImageDelegate(QObject* parent = nullptr);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
	void stateChanged(bool state, const QModelIndex& index);
private:
};

#endif // SIMAGEDELEGATE_H