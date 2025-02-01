#include "SUploadImagePage.h"
#include "SBigIconButton.h"
#include "SUploadSingleImageView.h"
#include "SUploadMultiImageView.h"
#include <QBoxLayout>

SUploadImagePage::SUploadImagePage(QWidget *parent) 
	: QWidget(parent)
{
	init();
	
}

SUploadImagePage::~SUploadImagePage()
{
}

void SUploadImagePage::init()
{	
	auto main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);
	
	//------------------------上传单张 --- 批量上传-----------------------------
	auto middle_layout = new QHBoxLayout;
	middle_layout->setContentsMargins(0, 0, 0, 0);
	auto uploadBtn = new SBigIconButton(":/ResourceClient/create.png", "单张上传", 10);
	uploadBtn->setMinimumSize(500, 500);
	auto uploadBtn_some = new SBigIconButton(":/ResourceClient/create.png", "批量上传", 10);
	uploadBtn_some->setFixedSize(500, 500);
	middle_layout->addStretch();
	middle_layout->addWidget(uploadBtn);
	middle_layout->addStretch();
	middle_layout->addWidget(uploadBtn_some);
	middle_layout->addStretch();

	main_layout->addStretch();
	main_layout->addLayout(middle_layout);
	main_layout->addStretch();

	this->setLayout(main_layout);

	main_layout->update();
	layout()->activate();


	connect(uploadBtn, &SBigIconButton::clicked, this, &SUploadImagePage::onUploadSingleImageClicked);
	connect(uploadBtn_some, &SBigIconButton::clicked, this, &SUploadImagePage::onUploadSomeImageClicked);
}

void SUploadImagePage::resizeEvent(QResizeEvent* event)
{
	if (m_uploadSingle) {
		m_uploadSingle->resize(this->size());
	}
	QWidget::resizeEvent(event);
}


void SUploadImagePage::onUploadSingleImageClicked() {
	if (!m_uploadSingle){
		m_uploadSingle = new SUploadSingleImageView(this);

	}
	m_uploadSingle->resize(this->size());
	m_uploadSingle->show();
}


void SUploadImagePage::onUploadSomeImageClicked() {
	if (!m_uploadMulti) {
		m_uploadMulti = new SUploadMultiImageView(this);

	}
	m_uploadMulti->resize(this->size());
	m_uploadMulti->show();
}