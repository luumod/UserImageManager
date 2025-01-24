#include "SImageManagerPage.h"
#include "SImageDetailView.h"
#include "SImageModifyView.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SHeaderView.h"
#include "SCheckDelegate.h"
#include "SUrlDelegate.h"
#include "SImageDelegate.h"
#include "SSwitchDelegate.h"
#include "SFieldTranslate.h"
#include "SSwitchButton.h"
#include "SButtonDelegate.h"
#include "SUserDetailView.h"
#include "SUserAddView.h"
#include "SUserEditView.h"
#include "SMaskWidget.h"
#include <QPushButton>
#include <set>
#include <QLineEdit>
#include <QTableView>
#include <QFormLayout>
#include <QComboBox>
#include <QRandomGenerator>

SImageManagerPage::SImageManagerPage(QWidget* parent)
	:QWidget(parent)
	,m_model(new QStandardItemModel(this))
{
	init();
}

SImageManagerPage::~SImageManagerPage()
{
}

void SImageManagerPage::init()
{
	SFieldTranslate::instance()->addTrans("image/", "");
	SFieldTranslate::instance()->addTrans("image/image_id", "图片编号");
	SFieldTranslate::instance()->addTrans("image/image_name", "图片名称");
	SFieldTranslate::instance()->addTrans("image/image_type", "图片类型");
	SFieldTranslate::instance()->addTrans("image/image_path", "图片");
	SFieldTranslate::instance()->addTrans("image/image_format", "图片格式");
	SFieldTranslate::instance()->addTrans("image/image_size", "图片尺寸");
	SFieldTranslate::instance()->addTrans("image/image_ResolutionRatio", "图片分辨率");
	SFieldTranslate::instance()->addTrans("image/image_share", "分享");
	SFieldTranslate::instance()->addTrans("image/image_download", "下载");
	SFieldTranslate::instance()->addTrans("image/isDeleted", "图片状态");
	SFieldTranslate::instance()->addTrans("image/operation", "操作");

	//----------顶部查询------------
	auto tipLab = new QLabel("筛选条件：");

	QFormLayout* nameLayout = new QFormLayout;
	m_searchEdit_imageName = new QLineEdit;
	m_searchEdit_imageName->setPlaceholderText("请输入图片名称（选填）");
	nameLayout->addRow(new QLabel("图片名称："), m_searchEdit_imageName);

	QFormLayout* typeLayout = new QFormLayout;
	m_searchEdit_imageType = new QLineEdit;
	m_searchEdit_imageType->setPlaceholderText("请输入图片类型（选填）");
	typeLayout->addRow(new QLabel("类型："), m_searchEdit_imageType);

	QFormLayout* formatLayout = new QFormLayout;
	m_searchCombo_imageFormat = new QComboBox;
	m_searchCombo_imageFormat->setPlaceholderText("请输入选择图片格式（选填）");
	m_searchCombo_imageFormat->addItem("任意");
	m_searchCombo_imageFormat->addItem("png");
	m_searchCombo_imageFormat->addItem("jpg");
	m_searchCombo_imageFormat->addItem("jpeg");
	m_searchCombo_imageFormat->setCurrentIndex(0);
	formatLayout->addRow(new QLabel("格式："), m_searchCombo_imageFormat);

	QFormLayout* sharedLayout = new QFormLayout;
	m_searchCombo_imageShare = new QComboBox;
	m_searchCombo_imageShare->addItem("私有"); //0私有
	m_searchCombo_imageShare->addItem("公有");//1公开
	m_searchCombo_imageShare->addItem("授权");//2授权
	m_searchCombo_imageShare->addItem("任意");
	m_searchCombo_imageShare->setPlaceholderText("请选择共享方式（选填）");
	m_searchCombo_imageShare->setCurrentIndex(3);
	sharedLayout->addRow(new QLabel("共享："), m_searchCombo_imageShare);

	QFormLayout* downloadLayout = new QFormLayout;
	m_searchCombo_imageDownload = new QComboBox;
	m_searchCombo_imageDownload->addItem("私有"); //0不可下载
	m_searchCombo_imageDownload->addItem("共有");   //1可下载
	m_searchCombo_imageDownload->addItem("授权");   //2授权下载
	m_searchCombo_imageDownload->addItem("任意");
	m_searchCombo_imageDownload->setPlaceholderText("请选择下载方式（选填）");
	m_searchCombo_imageDownload->setCurrentIndex(3);
	downloadLayout->addRow(new QLabel("下载："), m_searchCombo_imageDownload);

	auto toplayout = new QHBoxLayout;
	toplayout->addWidget(tipLab);
	toplayout->addLayout(nameLayout);
	toplayout->addLayout(typeLayout);
	toplayout->addLayout(formatLayout);
	toplayout->addLayout(sharedLayout);
	toplayout->addLayout(downloadLayout);
	//toplayout->addStretch();
	//------------------------------

	//------第二层：新增，删除，批量操作-------
	/*m_userAddBtn = new QPushButton("新增");
	m_batchEnableBtn = new QPushButton("批量启用");
	m_batchDisableBtn = new QPushButton("批量禁用");
	m_batchDeleteBtn = new QPushButton("批量删除");

	auto secondLayout = new QHBoxLayout;
	secondLayout->addWidget(m_userAddBtn);
	secondLayout->addWidget(m_batchEnableBtn);
	secondLayout->addWidget(m_batchDisableBtn);
	secondLayout->addWidget(m_batchDeleteBtn);
	secondLayout->addStretch();*/
	//------------------------------


	//------第三层：表格视图---------------
	m_tableView = new QTableView;
	//-----------------------------

	//------第四层：切换页面---------
	auto bottomLayout = new QHBoxLayout;
	auto totalLab = new QLabel;
	auto nextPageBtn = new QPushButton;
	nextPageBtn->setIcon(QPixmap(":/ResourceClient/next.png"));
	auto prevPageBtn = new QPushButton;
	prevPageBtn->setIcon(QPixmap(":/ResourceClient/pre.png"));
	auto pageLab = new QLabel;
	pageLab->setText(QString::number(m_currentPage)); //从1开始
	auto text1= new QLabel("前往");
	auto pageEdit = new QLineEdit;
	pageEdit->setFixedWidth(30);
	pageEdit->setAlignment(Qt::AlignCenter);
	pageEdit->setText(QString::number(m_currentPage));
	auto text2= new QLabel("页");
	auto totalPageLab = new QLabel;
	bottomLayout->addWidget(totalLab);
	bottomLayout->addWidget(prevPageBtn);
	bottomLayout->addWidget(pageLab);
	bottomLayout->addWidget(nextPageBtn);
	bottomLayout->addWidget(text1);
	bottomLayout->addWidget(pageEdit);
	bottomLayout->addWidget(text2);
	bottomLayout->addWidget(totalPageLab);
	bottomLayout->addStretch();
	//------------------------------

	//------主布局------------------
	auto mlayout = new QVBoxLayout;
	mlayout->addLayout(toplayout);
	//mlayout->addLayout(secondLayout);
	mlayout->addWidget(m_tableView);
	mlayout->addLayout(bottomLayout);
	setLayout(mlayout);


	//把模型交给视图显示：QTreeView - QStandardItemModel - QStandardItem
	m_tableView->setModel(m_model);
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_tableView->setGridStyle(Qt::NoPen);
	m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tableView->verticalHeader()->setVisible(false);
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_tableView->setFocusPolicy(Qt::NoFocus);	
	m_tableView->setMouseTracking(true);


	//-----------------搜索----------------------
	//按图片名称搜索
	connect(m_searchEdit_imageName, &QLineEdit::returnPressed, this, &SImageManagerPage::onSearch);
	connect(m_searchEdit_imageName, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });

	//按图片类型搜索
	connect(m_searchEdit_imageType, &QLineEdit::returnPressed, this, &SImageManagerPage::onSearch);
	connect(m_searchEdit_imageType, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });

	//按图片格式
	connect(m_searchCombo_imageFormat, &QComboBox::currentTextChanged, this, &SImageManagerPage::onSearch);

	//按共享方式搜索
	connect(m_searchCombo_imageShare, &QComboBox::currentTextChanged, this, &SImageManagerPage::onSearch);

	//按下载方式搜索
	connect(m_searchCombo_imageDownload, &QComboBox::currentTextChanged, this, &SImageManagerPage::onSearch);
	//------------------------------------------


	//-------------切换页面-------------------
	connect(this, &SImageManagerPage::totalRecordsCountChanged, this, [=](int count) {
		totalLab->setText(QString("共%1条记录").arg(count));
		checkPageValidity();
		});

	connect(this, &SImageManagerPage::totalPagesCountChanged, this, [=](int count) {
		totalPageLab->setText(QString("共%1页").arg(count));
		checkPageValidity();
		});

	connect(this, &SImageManagerPage::pageChanged, [=](int page) {
		pageLab->setText(QString::number(page));
		pageEdit->setText(QString::number(page));
		});

	//下一页
	connect(nextPageBtn, &QPushButton::released, this, [=]() {
		m_currentPage++;
		checkPageValidity();
		onSearch();
		});
	//上一页
	connect(prevPageBtn, &QPushButton::released, this, [=]() {
		m_currentPage--;
		checkPageValidity();
		onSearch();
		});
	//指定页
	connect(pageEdit, &QLineEdit::returnPressed, this, [=]() {
		m_currentPage = pageEdit->text().toInt();
		checkPageValidity();
		onSearch();
		});

	//设置表格视图的头视图
	auto hHeaderView = new SHeaderView(Qt::Horizontal);
	m_tableView->setHorizontalHeader(hHeaderView);
	
	//item代理: 复选框
	auto checkDelegate = new SCheckDelegate(m_tableView);
	m_tableView->setItemDelegateForColumn(0, checkDelegate);
	connect(hHeaderView, &SHeaderView::stateChanged, [=](int state) {
		for (int i = 0; i < m_model->rowCount(); i++) {
			auto item = m_model->item(i, 0);
			item->setData(state, Qt::UserRole);
		}
		if (state) {
			checkDelegate->setCheckCount(m_model->rowCount());
		}
		else {
			checkDelegate->setCheckCount(0);
		}
		});
	connect(checkDelegate, &SCheckDelegate::stateChanged, [=]() {
		int count = checkDelegate->checkCount();
		if (count == m_model->rowCount()) {
			hHeaderView->setState(Qt::Checked);
		}
		else if (count > 0) {
			hHeaderView->setState(Qt::PartiallyChecked);
		}
		else {
			hHeaderView->setState(Qt::Unchecked);
		}
	});

	//名称代理: image_name
	auto urlDelegate = new SUrlDelegate(m_tableView);
	m_tableView->setItemDelegateForColumn(column("image_name"), urlDelegate);
	//connect(urlDelegate, &SUrlDelegate::requestOpenUrl, this, &SImageManagerPage::showImageDetail);

	//图片显示代理: image_path
	auto imageDelegate = new SImageDelegate(m_tableView);
	m_tableView->setItemDelegateForColumn(column("image_path"), imageDelegate);

	//图片状态（删除或未删除）代理
	auto switchDelegate = new SSwitchDelegate(SSwitchDelegate::UseInWhat::ImageManager,m_tableView);
	m_tableView->setItemDelegateForColumn(column("isDeleted"), switchDelegate);
	connect(switchDelegate, &SSwitchDelegate::stateChanged, [=](int state, const QModelIndex& index) {
		QJsonObject jObj;
		QJsonArray jArray;
		auto isDeleted = !!m_model->data(index, Qt::UserRole).toInt();
		jArray.append(m_model->index(index.row(), column("image_id")).data().toInt());
		jObj.insert("lists", jArray);
		jObj.insert("isDeleted", isDeleted);
		SHttpClient(URL("/api/user/delete_orRestore_image")).debug(true)
			.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
			.json(jObj) // 要操作的图片image_id与isDeleted
			.fail([=](const QString& error, int code) {
#ifdef _DEBUG
			qCritical() << "error:" << error << "code:" << code;
#endif
				})
			.success([=](const QByteArray& data) {
			QJsonParseError error;
			auto jdom = QJsonDocument::fromJson(data, &error);
			if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
				qWarning() << "json数据解析错误: " << error.errorString();
#endif
			}
			if (jdom["code"].toInt() < 1000) {
				m_model->item(index.row(), column("isDeleted"))->setData(state, Qt::UserRole);
			}
				})
			.post();
		});

	//操作代理
	auto buttonDelegate = new SButtonDelegate(m_tableView);
	m_tableView->setItemDelegateForColumn(column("operation"), buttonDelegate);
	
	auto detailBtn = buttonDelegate->addButton(new QPushButton("详细"));
	auto modifyBtn = buttonDelegate->addButton(new QPushButton("修改"));
	auto delBtn = buttonDelegate->addButton(new QPushButton("删除"));

	connect(buttonDelegate, &SButtonDelegate::buttonClicked, [=](int id, QModelIndex Index) {
		if (id == 0) { //图片详细
			if (!m_imageDetailView) {
				m_imageDetailView = new SImageDetailView(SImageDetailView::ImageDetailPageType::In_SImageManagerPage
					, this);
			}
			//已知image_id,据此查询图片详细信息
			SHttpClient(URL("/api/user/get_one_image")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.params({ {"image_id" ,m_model->index(Index.row(),column("image_id")).data().toInt()} })
				.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
				qWarning() << msg << code;
#endif
					})
				.success([=](const QByteArray& data) {
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
					qWarning() << "Json解析错误: " << error.errorString();
#endif
					return;
				}
				if (jdom["code"].toInt() < 1000) {
					auto jImage = jdom["data"]["images"].toArray()[0].toObject();
					m_imageDetailView->setData(jImage);
					m_imageDetailView->resize(this->size());
					m_imageDetailView->show();
				}
				else {
#ifdef _DEBUG 
					qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
				}
					})
				.get(); // 发送get请求
		}
		else if (id == 1) { //图片修改
			if (!m_imageModifyView) {
				m_imageModifyView = new SImageModifyView(this);
				connect(m_imageModifyView, &SImageModifyView::onCloseAndModified, this, &SImageManagerPage::onSearch); //更新
			}
			SHttpClient(URL("/api/user/get_one_image")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.params({ {"image_id" ,m_model->index(Index.row(),column("image_id")).data().toInt()} })
				.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
				qWarning() << msg << code;
#endif
					})
				.success([=](const QByteArray& data) {
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
					qWarning() << "Json解析错误: " << error.errorString();
#endif
					return;
				}
				if (jdom["code"].toInt() < 1000) {
					auto jImage = jdom["data"]["images"].toArray()[0].toObject();
					m_imageModifyView->resize(this->size());
					m_imageModifyView->setData(jImage);
					m_imageModifyView->show();
				}
				else {
#ifdef _DEBUG 
					qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
				}
					})
				.get(); // 发送get请求
		}
		else if (id == 2) { //单张图片删除
			QJsonArray jArray;
			jArray.append(m_model->index(Index.row(), column("image_id")).data().toInt()); //要删除的image_id
			QJsonObject jData;
			jData.insert("lists", jArray);
			jData.insert("isDeleted", true); //是true而不是1
			SHttpClient(URL("/api/user/delete_orRestore_image")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json(jData)
				.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
				qWarning() << msg << code;
#endif
					})
				.success([=](const QByteArray& data) {
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
					qWarning() << "Json解析错误: " << error.errorString();
#endif
					return;
				}
				if (jdom["code"].toInt() < 1000) {
					onSearch();
				}
				else {
#ifdef _DEBUG 
					qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
				}
					})
				.post(); // 发送post请求
		}

		});

	//设置样式
	QString styleStr = R"(QPushButton{border-radius:20px; background-color: rgb(97,162,243);}
						QPushButton:hover {background-color: red;})";
	detailBtn->setStyleSheet(styleStr);
	modifyBtn->setStyleSheet(styleStr);
	delBtn->setStyleSheet(styleStr);
	//moreBtn->setStyleSheet(styleStr);
	detailBtn->setFixedSize(TableView_ButtonWidth, TableView_ButtonHeight);
	modifyBtn->setFixedSize(TableView_ButtonWidth, TableView_ButtonHeight);
	delBtn->setFixedSize(TableView_ButtonWidth, TableView_ButtonHeight);
}

void SImageManagerPage::onSearch()
{
	QVariantMap params;
	params.insert("user_account", sApp->userData("user/user_account").toString());
	params.insert("page", m_currentPage);
	params.insert("pageSize", m_pageSize);
	if (!m_searchEdit_imageName->text().isEmpty()) {
		params.insert("image_name", m_searchEdit_imageName->text());
	}
	if (!m_searchEdit_imageType->text().isEmpty()) {
		params.insert("image_type", m_searchEdit_imageType->text());
	}
	if (m_searchCombo_imageFormat->currentText() != "任意") {
		params.insert("image_format", m_searchCombo_imageFormat->currentText());
	}
	if (m_searchCombo_imageShare->currentText() != "任意") {
		params.insert("image_share", m_searchCombo_imageShare->currentIndex());
	}
	if (m_searchCombo_imageDownload->currentText() != "任意") {
		params.insert("image_download", m_searchCombo_imageDownload->currentIndex());
	}
	SHttpClient(URL("/api/user/get_image_all")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.params(params) // 参数形式 ?ddd=&xxx=
		.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
		qWarning() << msg << code;
#endif
			})
		.success([=](const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
#ifdef _DEBUG
			qWarning() << "Json解析错误: " << error.errorString();
#endif
			return;
		}
		if (jdom["code"].toInt() < 1000) {
			parseJson(jdom.object()); // 解析json数据
		}
		else {
#ifdef _DEBUG 
			qWarning() << "查询数据失败: " << jdom["message"].toString();
#endif
		}
			})
		.get(); // 发送get请求
}

void SImageManagerPage::showPage()
{
	onSearch();
}

void SImageManagerPage::parseJson(const QJsonObject& obj)
{
	m_model->clear();

	for (int i = 0; i < m_fieldName.size(); i++) {
		//获取user组下的字段名
		m_model->setHorizontalHeaderItem(i, new QStandardItem(SFieldTranslate::instance()->trans("image", m_fieldName[i])));
	}
	auto jArray = obj["data"]["images"].toArray();
	m_lastPage = obj["data"]["last_page"].toInt();
	m_totalCount = obj["data"]["total_records"].toInt();
	emit totalRecordsCountChanged(m_totalCount);
	emit totalPagesCountChanged(m_lastPage);

	for (int i = 0; i < jArray.size(); i++) {
		auto jImage = jArray[i].toObject();
		//往模型中插入数据
		m_model->appendRow(createItems(jImage));
		m_tableView->setRowHeight(i, 120); //每添加一行，设置行高为120（用于容纳图片显示）
	}
	m_tableView->setColumnWidth(column("operation"), 300);//TableView_ButtonWidth * 3 + 50
	dynamic_cast<SHeaderView*>(m_tableView->horizontalHeader())->setState(Qt::Unchecked);
}

void SImageManagerPage::checkPageValidity()
{
	if (m_currentPage > m_lastPage || m_currentPage < 1) {
		m_currentPage = 1;
	}

	emit pageChanged(m_currentPage);
}

int SImageManagerPage::column(const QString& field)
{
	auto p = m_fieldName.indexOf(field);
	if (p == -1) {
		qWarning() << "field" << field << "not exists.";
	}
	return p;
}

QList<QStandardItem*> SImageManagerPage::createItems(const QJsonObject& jImage)
{
	QList<QStandardItem*> items;
	for (const auto& field : m_fieldName) {
		auto item = new QStandardItem;
		if (field == "image_size") {
			item->setText(QString::number(jImage["image_size"].toInt() / 1024) + "KB");
		}
		else if (field == "image_share") {
			item->setText(jImage["image_share"].toInt() == 1 ? "公开" : jImage["image_share"].toInt() == 0 ? "私有" : "授权");
		}
		else if (field == "image_download") {
			item->setText(jImage["image_download"].toInt() == 1 ? "公开" : jImage["image_download"].toInt() == 0 ? "私有" : "授权");
		}
		else if (field == "image_path") {
			item->setData(jImage["image_path"].toString(),Qt::UserRole); //自定义代理
		}
		else if (field == "isDeleted") {
			item->setData(jImage["isDeleted"].toInt(), Qt::UserRole); //自定义代理
		}
		else {
			item->setText(jImage[field].toVariant().toString());
		}
		item->setTextAlignment(Qt::AlignCenter);
		items.append(item);
	}
	return items;
}
