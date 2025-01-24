#ifndef SURLDELEGATE_H
#define SURLDELEGATE_H

#include <QStyledItemDelegate>

class SUrlDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SUrlDelegate(QObject* parent = nullptr);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
	void requestOpenUrl(const QModelIndex& index);
};

#endif // SURLDELEGATE_H