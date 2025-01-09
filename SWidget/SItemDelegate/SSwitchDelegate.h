#ifndef SSITCHDELEGATE_H
#define SSITCHDELEGATE_H

#include <QStyledItemDelegate>

class SSwitchDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SSwitchDelegate(QObject* parent = nullptr);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
	void stateChanged(bool state, const QModelIndex& index);
private:
	QPoint m_mousePos{};
};

#endif // SSITCHDELEGATE_H