#include "SImageViewer.h"
#include <QFileInfo>
#include <QImageReader>
#include <QScrollArea>
#include <QScrollBar>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QDir>
#include <QPainter>
#include <QWheelEvent>
#include <QTimer>
#include <QLabel>

SImageViewer::SImageViewer(const QString& imagePath, QWidget* parent)
    : QMainWindow(parent)
{
    // 设置窗口属性
    setWindowTitle(QFileInfo(imagePath).fileName());
    setMinimumSize(800, 600);

    // 创建主部件
    scrollArea = new QScrollArea;
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    // 加载图片
    QImageReader reader(imagePath);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::critical(this, tr("Error"),
            tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(imagePath), reader.errorString()));
        return;
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();

    // 设置滚动区域
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    // 创建操作
    createActions();

    // 初始缩放适应窗口
    scaleImage(1.0);
    QSize imageSize = imageLabel->pixmap().size();
    QTimer::singleShot(0, this, &SImageViewer::fitToWindow); // 延迟执行确保窗口尺寸正确
}

void SImageViewer::createActions()
{
    // 创建菜单
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    // 缩放操作
    QAction* zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &SImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);

    QAction* zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &SImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);

    QAction* normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &SImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+0"));

    viewMenu->addSeparator();

    QAction* fullScreenAct = viewMenu->addAction(tr("&Full Screen"), this, &SImageViewer::toggleFullScreen);
    fullScreenAct->setShortcut(tr("F11"));

    QAction* fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, [this]() {
        fitToWindowEnabled = !fitToWindowEnabled;
        if (fitToWindowEnabled) {
            fitToWindow();
        }
        else {
            normalSize();
        }
        });
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setChecked(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

// 缩放功能实现
void SImageViewer::zoomIn() { scaleImage(1.25); }
void SImageViewer::zoomOut() { scaleImage(0.8); }

void SImageViewer::normalSize()
{
    fitToWindowEnabled = false;
    imageLabel->adjustSize();
    scaleFactor = 1.0;
    scrollArea->ensureVisible(imageLabel->width() / 2, imageLabel->height() / 2);
}

void SImageViewer::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
        menuBar()->show();
    }
    else {
        showFullScreen();
        menuBar()->hide();
    }
}

void SImageViewer::scaleImage(double factor)
{
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap().size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void SImageViewer::adjustScrollBar(QScrollBar* scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
        + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void SImageViewer::fitToWindow()
{
    QSize imageSize = imageLabel->pixmap().size();
    QSize windowSize = scrollArea->size();

    // 计算适合窗口的缩放比例
    double hScale = (windowSize.width() - 20) * 1.0 / imageSize.width();
    double vScale = (windowSize.height() - 20) * 1.0 / imageSize.height();
    scaleFactor = qMin(hScale, vScale);

    imageLabel->resize(scaleFactor * imageSize);
    fitToWindowEnabled = true;

    // 保持图片居中
    scrollArea->ensureVisible(imageLabel->width() / 2, imageLabel->height() / 2);
}

// 鼠标事件处理
void SImageViewer::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
        event->accept();
    }
    else {
        QMainWindow::wheelEvent(event);
    }
}

void SImageViewer::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        toggleFullScreen();
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

void SImageViewer::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (fitToWindowEnabled) {
        fitToWindow();
    }
}