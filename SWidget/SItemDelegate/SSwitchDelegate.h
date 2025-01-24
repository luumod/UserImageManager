#ifndef SSITCHDELEGATE_H
#define SSITCHDELEGATE_H

#include <QStyledItemDelegate>

class SSwitchDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	enum class UseInWhat {
		UserManager,
		ImageManager,
	};
	SSwitchDelegate(UseInWhat use = UseInWhat::UserManager, QObject* parent = nullptr);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
	void stateChanged(bool state, const QModelIndex& index);
private:
	QPoint m_mousePos{};

	UseInWhat m_useInWhat{ UseInWhat::UserManager };
};

#endif // SSITCHDELEGATE_H