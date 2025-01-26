#ifndef SBUTTONDELEGATE_H
#define SBUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include <QAbstractButton>
#include <vector>

class QAbstractButton;
class SButtonDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SButtonDelegate(QObject* parent = nullptr);
	QAbstractButton* addButton(QAbstractButton* button);
	QAbstractButton* button(int id);
	int buttonTotalWidth()const;
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
	void buttonClicked(int id, const QModelIndex& index);
private:
	std::vector<std::unique_ptr<QAbstractButton>> m_buttons; 
	int m_spacing = 5; //两个按钮之间的间隔
	QPoint m_mousePos{};
};

#endif // SBUTTONDELEGATE_H