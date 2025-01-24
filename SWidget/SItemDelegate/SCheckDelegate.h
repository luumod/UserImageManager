#ifndef SCHECKDELEGATE_H
#define SCHECKDELEGATE_H

#include <QStyledItemDelegate>

class SCheckDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SCheckDelegate(QObject *parent = nullptr);
    int checkCount() const {return m_checkCount;}
    void setCheckCount(int count) { m_checkCount = count; }
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)override;
signals:
    void stateChanged(int state, const QModelIndex& model);
private:
    int m_checkCount{}; //选中的item数量
};

#endif // SCHECKDELEGATE_H