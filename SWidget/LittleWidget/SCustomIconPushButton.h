﻿#ifndef SCUSTOMICONPUSHBUTTON_H
#define SCUSTOMICONPUSHBUTTON_H

#include <QPushButton>

class SCustomIconPushButton : public QPushButton {
    Q_OBJECT

        Q_PROPERTY(qreal rotationAngle READ rotationAngle WRITE setRotationAngle NOTIFY rotationAngleChanged)

private:
    int radius;
    qreal widgetRatio;
    qreal iconSizeRate = 0.8;
    qreal rotation = 0;
    QPixmap* iconImg;
    QString iconHint;

    /* for hover and click effects */
    QColor bgColor;
    QColor defaultColor = QColor(0, 0, 0, 0);
    QColor hoverColor = QColor(241, 241, 241, 200);

protected:
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

protected:
    qreal rotationAngle() const { return rotation; }

public:
    SCustomIconPushButton(QString iconPath, QString hint = "", int r = 0, QWidget* parent = nullptr);
    SCustomIconPushButton(const QPixmap& icon, QString hint = "", int r = 0, QWidget* parent = nullptr);

    void setRotationAngle(qreal angle = 0) { rotation = angle; update(); }

signals:
    void rotationAngleChanged();
};
#endif // !SCUSTOMICONPUSHBUTTON_H
