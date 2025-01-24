#ifndef SM_MASK_WIDGET_H
#define SM_MASK_WIDGET_H

#include <QWidget>
#include <QEvent>

class SMaskWidget : public QWidget {
    Q_OBJECT
public:
    enum PopPosition {
        LeadingPosition,
        MiddlePosition,
        TrailingPosition
    };
public:
    SMaskWidget(QWidget* parent = 0);

    static SMaskWidget* instance();
    void setMainWindow(QWidget* mainWindow);

    void addDialog(QWidget* dialog);
    void popup(QWidget* dialog,PopPosition position = PopPosition::TrailingPosition);
protected:
    //事件过滤器
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event)override;
    void onResize();
private:
    QWidget* m_mainWindow{};
    QWidgetSet m_dialogs;
    QWidget* m_currentDialog{};
    PopPosition m_position{PopPosition::TrailingPosition};
};

#endif // SM_MASK_WIDGET_H

