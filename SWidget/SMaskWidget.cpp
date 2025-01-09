#include "SMaskWidget.h"
#include <QDebug>
#include <QKeyEvent>

SMaskWidget::SMaskWidget(QWidget* parent)
	: QWidget(parent, Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus)
{
	setStyleSheet("background-color: rgba(194,195,201,180);");
	setAttribute(Qt::WA_StyledBackground);
}

SMaskWidget* SMaskWidget::instance()
{
	static SMaskWidget* ins = nullptr;
	if (!ins)
		ins = new SMaskWidget;
	return ins;
}

void SMaskWidget::setMainWindow(QWidget* mainWindow)
{
	if (!m_mainWindow) {
		qWarning() << "m_mainWindow not set!";
	}
	if (m_mainWindow) {
		qWarning() << "m_mainWindow already set! will be overwritten!";
	}
	m_mainWindow = mainWindow;

	this->setParent(m_mainWindow);
	this->hide(); //首先隐藏
	m_mainWindow->installEventFilter(this);
}

void SMaskWidget::addDialog(QWidget* dialog)
{
	if (!dialog) {
		qWarning() << "dialog is null!";
		return;
	}
	auto it = m_dialogs.find(dialog);
	if (it != m_dialogs.end()) {
		qWarning()<<"dialog already added!";
		return;
	}
	m_dialogs.insert(dialog);
	dialog->installEventFilter(this);
}

void SMaskWidget::popup(QWidget* dialog, PopPosition position)
{
	addDialog(dialog);
	if (dialog && m_dialogs.contains(dialog)) {
		if (m_currentDialog)
			m_currentDialog->setParent(nullptr);
		m_currentDialog = dialog; //保存当前弹出窗口
		m_position = position; //保存弹出位置

		onResize();
		dialog->setParent(this);
		dialog->setFocus(); //弹出窗口获得焦点
		this->show();
		dialog->show();
	}
}

bool SMaskWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_mainWindow) {
		if (event->type() == QEvent::Resize) {
			this->setGeometry(m_mainWindow->rect());
		}
	}
	else {
		//弹出框
		if (event->type() == QEvent::Hide) {
			this->hide();
		}
		else if (event->type() == QEvent::Resize) {
			//如果自己的高度改变，则强制调整自己的高度
			onResize();
		}
		else if (event->type() == QEvent::KeyRelease) {
			auto key = static_cast<QKeyEvent*>(event);
			if (key->key() == Qt::Key_Escape) { //Esc关闭弹出框
				this->hide();
			}
		}
	}
	return false;
}

void SMaskWidget::resizeEvent(QResizeEvent* event)
{
	onResize();
}

void SMaskWidget::onResize()
{
	if (m_currentDialog) {
		switch (m_position)
		{
		case PopPosition::LeadingPosition:
			m_currentDialog->move(0, 0);
			m_currentDialog->setFixedHeight(this->height());
			break;
		case PopPosition::MiddlePosition:
			m_currentDialog->move((m_mainWindow->width() - m_currentDialog->width()) / 2, (m_mainWindow->height() - m_currentDialog->height()) / 2);
			m_currentDialog->setFixedHeight(this->height());
			break;
		case PopPosition::TrailingPosition:
			m_currentDialog->move(m_mainWindow->width() - m_currentDialog->width(), 0);
			m_currentDialog->setFixedHeight(this->height());
			break;
		default:
			break;
		}
	}
	
		
}
