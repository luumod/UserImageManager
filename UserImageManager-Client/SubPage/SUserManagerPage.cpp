#include "SUserManagerPage.h"
#include "SUserManagerPage.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SHeaderView.h"
#include "SCheckDelegate.h"
#include "SUrlDelegate.h"
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

SUserManagerPage::SUserManagerPage(QWidget* parent)
	:QWidget(parent)
	,m_model(new QStandardItemModel(this))
{
	init();
}

SUserManagerPage::~SUserManagerPage()
{
}

void SUserManagerPage::init()
{
	SFieldTranslate::instance()->addTrans("user/", "");
	SFieldTranslate::instance()->addTrans("user/id", "用户编号");
	SFieldTranslate::instance()->addTrans("user/user_account", "账号");
	SFieldTranslate::instance()->addTrans("user/user_name", "用户昵称");
	SFieldTranslate::instance()->addTrans("user/gender", "性别");
	SFieldTranslate::instance()->addTrans("user/email", "邮箱");
	SFieldTranslate::instance()->addTrans("user/mobile", "电话");
	SFieldTranslate::instance()->addTrans("user/isEnable", "账号状态");
	SFieldTranslate::instance()->addTrans("user/operation", "操作");

	//----------顶部查询------------
	QFormLayout* idLayout = new QFormLayout;
	m_searchEdit_userid = new QLineEdit;
	m_searchEdit_userid->setPlaceholderText("请输入用户账号（选填）");
	idLayout->addRow(new QLabel("id："), m_searchEdit_userid);

	QFormLayout* nameLayout = new QFormLayout;
	m_searchEdit_username = new QLineEdit;
	m_searchEdit_username->setPlaceholderText("请输入用户名（选填）");
	nameLayout->addRow(new QLabel("昵称："), m_searchEdit_username);

	QFormLayout* mobileLayout = new QFormLayout;
	m_searchEdit_mobile = new QLineEdit;
	m_searchEdit_mobile->setPlaceholderText("请输入电话（选填）");
	mobileLayout->addRow(new QLabel("电话："), m_searchEdit_mobile);

	QFormLayout* emailLayout = new QFormLayout;
	m_searchEdit_email = new QLineEdit;
	m_searchEdit_email->setPlaceholderText("请输入邮箱（选填）");
	emailLayout->addRow(new QLabel("邮箱："), m_searchEdit_email);

	QFormLayout* genderdLayout = new QFormLayout;
	m_searchCombo_gender = new QComboBox;
	m_searchCombo_gender->addItem("男");
	m_searchCombo_gender->addItem("女");
	m_searchCombo_gender->addItem("未知");
	m_searchCombo_gender->addItem("任意");
	m_searchCombo_gender->setPlaceholderText("请选择性别（选填）");
	m_searchCombo_gender->setCurrentIndex(3);
	genderdLayout->addRow(new QLabel("性别："), m_searchCombo_gender);

	m_searchBtn = new QPushButton("查询");

	auto toplayout = new QHBoxLayout;
	toplayout->addLayout(idLayout);
	toplayout->addLayout(nameLayout);
	toplayout->addLayout(mobileLayout);
	toplayout->addLayout(emailLayout);
	toplayout->addLayout(genderdLayout);
	toplayout->addWidget(m_searchBtn);
	toplayout->addStretch();
	//------------------------------

	//------第二层：新增，删除，批量操作-------
	m_userAddBtn = new QPushButton("新增");
	m_batchEnableBtn = new QPushButton("批量启用");
	m_batchDisableBtn = new QPushButton("批量禁用");
	m_batchDeleteBtn = new QPushButton("批量删除");

	auto secondLayout = new QHBoxLayout;
	secondLayout->addWidget(m_userAddBtn);
	secondLayout->addWidget(m_batchEnableBtn);
	secondLayout->addWidget(m_batchDisableBtn);
	secondLayout->addWidget(m_batchDeleteBtn);
	secondLayout->addStretch();
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
	mlayout->addLayout(secondLayout);
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
	//m_tableView->setStyleSheet("\
	//	QTableView{ \
	//		border: none;\
	//		background-color: red;\
	//		/*background-color: rgb(236,235,236);*/\
	//		selection-background-color: rgb(220, 220, 220);\
	//	}\
	//	QTableView QTableCornerButton::section {\
	//	background: red;\
	//	border: 2px outset red;\
	//	}\
	//	QTableView::indicator:unchecked {\
	//	background - color: #d7d6d5\
	//	}\
	//	QHeaderView::section{    \
	//		border: none;\
	//		height:30px;   \
	//		font: 15px soild black;\
	//		font-weight: 600;\
	//		background-color: rgb(130,181,208);\
	//		text-align: center;\
	//	}\
	//	QTableView::item{ \
	//		border: none;\
	//		/*background-color: rgb(220, 220, 220);*/ \
	//		selection-color: black;\
	//		font-family: Consolas;\
	//		font-size: 20px;\
	//	}\
	//	QTableView::item:!alternate : !selected{\
	//		background-color: white; \
	//		selection-background-color: rgb(220, 220, 220);\
	//		selection-color: red;\
	//	}");
	//m_tableView->setShowGrid(false); 
	//m_tableView->setAlternatingRowColors(true); //<-----双色显示")

	////设置每一个item的高度
	//m_tableView->verticalHeader()->setDefaultSectionSize(50);
	//m_tableView->horizontalHeader()->setDefaultSectionSize(150);

	//添加用户
	connect(m_userAddBtn, &QPushButton::released, this, [=]() {
		if (!m_userAddDlg) {
			m_userAddDlg = new SUserAddView;
			connect(m_userAddDlg, &SUserAddView::newUser, [=](const QJsonObject& jUser) {
					m_model->insertRow(m_model->rowCount(), createItems(jUser));
					m_userAddDlg->close();

					onSearch();
				});
		}
		SMaskWidget::instance()->popup(m_userAddDlg);
		});

	//账号搜索
	connect(m_searchEdit_userid, &QLineEdit::returnPressed, this, &SUserManagerPage::onSearch);
	connect(m_searchEdit_userid, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });

	//名称搜索
	connect(m_searchEdit_username, &QLineEdit::returnPressed, this, &SUserManagerPage::onSearch);
	connect(m_searchEdit_username, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch();});

	//电话搜索
	connect(m_searchEdit_mobile, &QLineEdit::returnPressed, this, &SUserManagerPage::onSearch);
	connect(m_searchEdit_mobile, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });

	//邮箱搜索
	connect(m_searchEdit_email, &QLineEdit::returnPressed, this, &SUserManagerPage::onSearch);
	connect(m_searchEdit_email, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch(); });

	//性别搜索
	connect(m_searchCombo_gender, &QComboBox::currentTextChanged, this, &SUserManagerPage::onSearch);

	//查找按钮
	connect(m_searchBtn, &QPushButton::released, this, &SUserManagerPage::onSearch);

	//批量操作
	connect(m_batchEnableBtn, &QPushButton::released, this, &SUserManagerPage::onBatchEnable);
	connect(m_batchDisableBtn, &QPushButton::released, this, &SUserManagerPage::onBatchDisable);
	connect(m_batchDeleteBtn, &QPushButton::released, this, &SUserManagerPage::onBatchDelete);
	


	//底部
	connect(this, &SUserManagerPage::totalRecordsCountChanged, this, [=](int count) {
		totalLab->setText(QString("共%1条记录").arg(count));
		checkPageValidity();
		});

	connect(this, &SUserManagerPage::totalPagesCountChanged, this, [=](int count) {
		totalPageLab->setText(QString("共%1页").arg(count));
		checkPageValidity();
		});

	connect(this, &SUserManagerPage::pageChanged, [=](int page) {
		pageLab->setText(QString::number(page));
		pageEdit->setText(QString::number(page));
		});

	//下一页
	connect(nextPageBtn, &QPushButton::released, this, [=](){
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
	m_tableView->setItemDelegateForColumn(0, checkDelegate); //设置第一列的代理为复选框delegate

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

	//url代理: user_account
	auto urlDelegate = new SUrlDelegate(m_tableView);
	m_tableView->setItemDelegateForColumn(column("user_account"), urlDelegate);
	connect(urlDelegate, &SUrlDelegate::requestOpenUrl, this, &SUserManagerPage::showUserDetails);

	//开关代理: isEnable
	auto switchDelegate = new SSwitchDelegate(SSwitchDelegate::UseInWhat::UserManager,m_tableView);
	m_tableView->setItemDelegateForColumn(column("isEnable"), switchDelegate);
	connect(switchDelegate, &SSwitchDelegate::stateChanged, [=](int state,const QModelIndex& index) {

		auto user_account = m_model->item(index.row(), column("user_account"))->text();
		auto isEnable = !!m_model->data(index, Qt::UserRole).toInt();
		SHttpClient(URL("/api/user/alter?user_account=" + user_account)).debug(true)
			.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
			.json({ {"isEnable",isEnable} }) // json body数据
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
					m_model->item(index.row(), column("isEnable"))->setData(state, Qt::UserRole);
				}
				else {
#ifdef _DEBUG
					qInfo() << "禁用用户失败，用户id：" << user_account;
#endif
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

	//多种操作：详细信息，修改，删除
	connect(buttonDelegate, &SButtonDelegate::buttonClicked, this, [=](int id,const QModelIndex& index) {
		if (id == 0) { //详细
			showUserDetails(index);
		}
		else if (id == 1) { //修改
			showUserEditDlg(index);
		}
		else if (id == 2) { //删除
			auto user_account = m_model->item(index.row(), column("user_account"))->text(); 
			QJsonArray jArray;
			jArray.append(user_account); //list
			SHttpClient(URL("/api/user/delete")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ {"lists",jArray} })
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
						m_model->removeRow(index.row());
					}
					else {
#ifdef _DEBUG
						qInfo() << "删除用户失败，用户id：" << jArray;
#endif
					}
					})
				.post();

		}
		});
}

void SUserManagerPage::showPage()
{
	onSearch();
}

void SUserManagerPage::setBatchEnabeld(bool enable)
{
	QJsonObject jObj;
	QJsonArray jArray;
	QSet<QString> st;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i);
		if (item && item->data(Qt::UserRole).toBool()) { //(i,0)的复选框被选中
			auto isEnable = !!m_model->item(i, column("isEnable"))->data(Qt::UserRole).toInt(); //将值转为bool
			auto user_account = m_model->item(i, column("user_account"))->text(); //获取user_account这一列的值
			if ( isEnable != enable) { //true -> false  false -> true
				st.insert(user_account);
				QJsonObject recObj; 
				QJsonObject filterObj{ {"user_account",user_account} };
				QJsonObject updateObj{ {"isEnable",enable} };
				recObj.insert("filter", filterObj);
				recObj.insert("update", updateObj);
				jArray.append(recObj);

				//直接更新
				m_model->item(i, column("isEnable"))->setData(enable, Qt::UserRole);
			}
		}
	}
	/*
	Json格式如下：
	{
		lists:[
			{
				filter:{
					"user_account": 123456
				}
				update:{
					"isEnable": true
				}
			},
			{
				filter:{
					"user_account": 555666
				}
				update:{
					"isEnable": false
				}
			},
		]
	}
	
	*/

	jObj.insert("lists", jArray);
	SHttpClient(URL("/api/user/batch_alter")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jObj)
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
			qWarning() << "Json解析错误: " << error.errorString();
#endif
		}
		auto str = enable ? "启用" : "禁用";
		if (jdom["code"].toInt() < 1000) {
			//批量操作用户成功
#ifdef _DEBUG 
			qInfo() << "批量" << str << "用户成功，用户id：" << st;
#endif
		}
		else {
#ifdef _DEBUG
			qInfo() << "批量" << str << "用户失败，用户id：" << st;
#endif 
		}
			})
		.post();
}

void SUserManagerPage::showUserDetails(const QModelIndex& index)
{
	if (!m_detailsDlg) {
		m_detailsDlg = new SUserDetailView(this);
		connect(m_detailsDlg, &SUserDetailView::userChanged,this, [=](const QJsonObject& juser) {
			//只能禁用用户
			m_model->item(m_currentIndex.row(), column("isEnable"))->setData(juser.value("isEnable").toBool(), Qt::UserRole);
			sApp->updateUserData(juser);
			});
	}

	m_currentIndex = index; //保存当前详细信息的行索引
	QJsonObject jUser;
	jUser.insert("user_account", m_model->item(index.row(), column("user_account"))->text());
	jUser.insert("user_name", m_model->item(index.row(), column("user_name"))->text());
	jUser.insert("gender", m_model->item(index.row(), column("gender"))->text()=="男" ? 1 : (m_model->item(index.row(), column("gender"))->text()=="女" ? 2 : 0));
	jUser.insert("email", m_model->item(index.row(), column("email"))->text());
	jUser.insert("mobile", m_model->item(index.row(), column("mobile"))->text());
	jUser.insert("isEnable", m_model->item(index.row(), column("isEnable"))->data(Qt::UserRole).toBool());

	m_detailsDlg->resize(this->size());
	m_detailsDlg->setUser(jUser);
	m_detailsDlg->show();
}

void SUserManagerPage::showUserEditDlg(const QModelIndex& index)
{
	if (!m_userEditDlg)
	{
		m_userEditDlg = new SUserEditView(this);
		connect(m_userEditDlg, &SUserEditView::userChanged,this, [=](const QJsonObject& juser){
			//更新视图
			m_model->item(m_currentIndex.row(), column("user_name"))->setText(juser.value("user_name").toString());
			m_model->item(m_currentIndex.row(), column("email"))->setText(juser.value("email").toString());
			m_model->item(m_currentIndex.row(), column("mobile"))->setText(juser.value("mobile").toString());

			sApp->updateUserData(juser);
			});
	}

	m_currentIndex = index; //保存当前修改的行索引
	QJsonObject jUser;
	jUser.insert("user_account", m_model->item(index.row(), column("user_account"))->text());
	jUser.insert("user_name", m_model->item(index.row(), column("user_name"))->text());
	jUser.insert("email", m_model->item(index.row(), column("email"))->text());
	jUser.insert("mobile", m_model->item(index.row(), column("mobile"))->text());
	jUser.insert("isEnable", m_model->item(index.row(), column("isEnable"))->data(Qt::UserRole).toBool());

	m_userEditDlg->setUser(jUser);
	SMaskWidget::instance()->popup(m_userEditDlg);
}

void SUserManagerPage::resizeEvent(QResizeEvent* event)
{
	if (m_detailsDlg) {
		m_detailsDlg->resize(this->size());
	}
}

void SUserManagerPage::onBatchEnable()
{
	setBatchEnabeld(true);
}

void SUserManagerPage::onBatchDisable()
{
	setBatchEnabeld(false);
}

void SUserManagerPage::onBatchDelete()
{
	QJsonObject jObj;
	QJsonArray jArray;
	std::set<int, std::greater<>> deleteRows;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i);
		if (item && item->data(Qt::UserRole).toBool()) { //复选框被选中
			jArray.append(m_model->item(i,column("user_account"))->text()); //user_account
			deleteRows.insert(i);
		}
	}
	jObj.insert("lists", jArray); 
	SHttpClient(URL("/api/user/delete")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jObj)  //json body数据： { "lists": [xxx,xxx]}
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
				else {
					if (jdom["code"].toInt() < 1000) {
						for (const auto& row : deleteRows) {
							m_model->removeRow(row);
						}
					}
					else { //其他错误
#ifdef _DEBUG
						qWarning() << "删除数据失败: " << jdom["message"].toString();
#endif
					}
				}
			})
		.post();
}

void SUserManagerPage::onSearch()
{
	QVariantMap params;
	params.insert("isDeleted", false);
	params.insert("page", m_currentPage);
	params.insert("pageSize", m_pageSize);
	if (!m_searchEdit_userid->text().isEmpty()) {
		params.insert("user_account", m_searchEdit_userid->text());
	}
	if (!m_searchEdit_username->text().isEmpty()) {
		params.insert("user_name", m_searchEdit_username->text());
	}
	if (!m_searchEdit_email->text().isEmpty()) {
		params.insert("email", m_searchEdit_email->text());
	}
	if (!m_searchEdit_mobile->text().isEmpty()) {
		params.insert("mobile", m_searchEdit_mobile->text());
	}
	if (m_searchCombo_gender->currentText() != "任意") {
		params.insert("gender", m_searchCombo_gender->currentText());
	}
	SHttpClient(URL("/api/user/list")).debug(true)
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

void SUserManagerPage::parseJson(const QJsonObject& obj)
{
	qDebug() << "***********************   parseJson ********************************";
	m_model->clear();

	for (int i = 0; i < m_fieldName.size(); i++) {
		//获取user组下的字段名
		m_model->setHorizontalHeaderItem(i, new QStandardItem(SFieldTranslate::instance()->trans("user", m_fieldName[i])));
	}
	auto jArray = obj["data"]["list"].toArray();
	m_lastPage = obj["data"]["last_page"].toInt();
	m_totalCount = obj["data"]["total_records"].toInt();
	//当模糊查找时，底部的页数及记录数信息显示的是（对于此查找结果）的对应值，需要及时更新
	//若无模糊查找，则对应值就是全部记录的值
	emit totalRecordsCountChanged(m_totalCount); 
	emit totalPagesCountChanged(m_lastPage);

	for (int i = 0; i < jArray.size(); i++) {
		auto jUser = jArray[i].toObject();
		//往模型中插入数据
		m_model->appendRow(createItems(jUser));
		
	}
	m_tableView->setColumnWidth(column("operation"), 300);//TableView_ButtonWidth * 3 + 50
	dynamic_cast<SHeaderView*>(m_tableView->horizontalHeader())->setState(Qt::Unchecked);
}

void SUserManagerPage::checkPageValidity()
{
	if (m_currentPage > m_lastPage || m_currentPage < 1) {
		m_currentPage = 1;
	}

	emit pageChanged(m_currentPage);
}

int SUserManagerPage::column(const QString& field)
{
	auto p = m_fieldName.indexOf(field);
	if (p == -1) {
		qWarning() << "field" << field << "not exists.";
	}
	return p;
}

QList<QStandardItem*> SUserManagerPage::createItems(const QJsonObject& jUser)
{
	QList<QStandardItem*> items;
	auto gender = jUser["gender"].toInt();
	auto isEnable = jUser["isEnable"].toInt();
	for (const auto& field : m_fieldName) {
		auto item = new QStandardItem;
		if (field == "gender") {
			item->setText(gender == 0 ? "未知": (gender == 1 ? "男" : "女"));
		}
		else if (field == "isEnable") {
			item->setData(!!isEnable, Qt::UserRole);
			item->setText(isEnable ? "启用" : "禁用");
		}
		else {
			item->setText(jUser[field].toVariant().toString());
		}
		item->setTextAlignment(Qt::AlignCenter);
		items.append(item);
	}
	return items;
}
