#include "UserManagerPage.h"
#include "ui_UserManagerPage.h"
#include "SHttpClient.h"
#include "SApp.h"
#include "SHeaderView.h"
#include "SCheckDelegate.h"
#include "SUrlDelegate.h"
#include "SSwitchDelegate.h"
#include "SFieldTranslate.h"
#include "SSwitchButton.h"
#include "SButtonDelegate.h"
#include "UserDetailsDlg.h"
#include "UserAddDlg.h"
#include "UserEditDlg.h"
#include "SMaskWidget.h"
#include <QPushButton>
#include <set>

UserManagerPage::UserManagerPage(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::User_ManagerPage)
	,m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	init();

	/*auto hlayout = dynamic_cast<QHBoxLayout*>(ui->batchEnableBtn->parentWidget()->layout());
	if (hlayout) {
		hlayout->insertWidget(hlayout->count() - 1, new SSwitchButton);
	}*/
}

UserManagerPage::~UserManagerPage()
{
	delete ui;
}

void UserManagerPage::init()
{
	SFieldTranslate::instance()->addTrans("user/user_id", "用户ID");
	SFieldTranslate::instance()->addTrans("user/user_name", "用户名");
	SFieldTranslate::instance()->addTrans("user/gender", "性别");
	SFieldTranslate::instance()->addTrans("user/email", "邮箱");
	SFieldTranslate::instance()->addTrans("user/mobile", "电话");
	SFieldTranslate::instance()->addTrans("user/isEnable", "账号状态");


	//把模型交给视图显示：QTreeView - QStandardItemModel - QStandardItem
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableView->setGridStyle(Qt::NoPen);
	//ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableView->verticalHeader()->setVisible(false);
	ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableView->setFocusPolicy(Qt::NoFocus);	
	ui->tableView->setMouseTracking(true);
	//ui->tableView->setStyleSheet("\
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
	//ui->tableView->setShowGrid(false); 
	//ui->tableView->setAlternatingRowColors(true); //<-----双色显示")

	////设置每一个item的高度
	//ui->tableView->verticalHeader()->setDefaultSectionSize(50);
	//ui->tableView->horizontalHeader()->setDefaultSectionSize(150);

	connect(ui->userAddBtn, &QPushButton::clicked, this, [=]() {
		if (!m_userAddDlg) {
			m_userAddDlg = new UserAddDlg;
			//SMaskWidget::instance()->addDialog(m_userAddDlg);
			connect(m_userAddDlg, &UserAddDlg::newUser, [=](const QJsonObject& jUser) {
					m_model->insertRow(m_model->rowCount(), createItems(jUser));
					m_userAddDlg->close();
				});
		}
		//m_userAddDlg->show();
		SMaskWidget::instance()->popup(m_userAddDlg);
		});
	


	connect(ui->searchEdit, &QLineEdit::returnPressed, this, &UserManagerPage::onSearch);
	connect(ui->searchEdit, &QLineEdit::textChanged, this, [=](const QString& text) {if (text.isEmpty()) onSearch();});
	connect(ui->searchBtn, &QPushButton::clicked, this, &UserManagerPage::onSearch);
	connect(ui->batchEnableBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchEnable);
	connect(ui->batchDisableBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchDisable);
	connect(ui->batchDeleteBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchDelete);

	onSearch();

	//设置表格视图的头视图
	auto hHeaderView = new SHeaderView(Qt::Horizontal);
	ui->tableView->setHorizontalHeader(hHeaderView);
	
	//item代理: 复选框
	auto checkDelegate = new SCheckDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(0, checkDelegate); //设置第一列的代理为复选框delegate

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

	//url代理: user_id
	auto urlDelegate = new SUrlDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("user_id"), urlDelegate);
	connect(urlDelegate, &SUrlDelegate::requestOpenUrl, this, &UserManagerPage::showUserDetails);

	//开关代理: isEnable
	auto switchDelegate = new SSwitchDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("isEnable"), switchDelegate);
	connect(switchDelegate, &SSwitchDelegate::stateChanged, [=](int state,const QModelIndex& index) {

		auto user_id = m_model->item(index.row(), column("user_id"))->text();
		auto isEnable = !!m_model->data(index, Qt::UserRole).toInt();
		SHttpClient(URL("/api/user/alter?user_id=" + user_id)).debug(true)
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
					qInfo() << "禁用用户失败，用户id：" << user_id;
#endif
				}
			})
			.post();
		});

	//操作代理
	auto buttonDelegate = new SButtonDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("operation"), buttonDelegate);
	
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

	connect(buttonDelegate, &SButtonDelegate::buttonClicked, this, [=](int id,const QModelIndex& index) {
		if (id == 0) { //详细
			showUserDetails(index);
		}
		else if (id == 1) { //修改
			showUserEditDlg(index);
		}
		else if (id == 2) { //删除
			auto user_id = m_model->item(index.row(), column("user_id"))->text(); 
			QJsonArray jArray;
			jArray.append(user_id); //list
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

void UserManagerPage::setBatchEnabeld(bool enable)
{
	QJsonObject jObj;
	QJsonArray jArray;
	QSet<QString> st;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i);
		if (item && item->data(Qt::UserRole).toBool()) { //(i,0)的复选框被选中
			auto isEnable = !!m_model->item(i, column("isEnable"))->data(Qt::UserRole).toInt(); //将值转为bool
			auto user_id = m_model->item(i, column("user_id"))->text(); //获取user_id这一列的值
			if ( isEnable != enable) { //true -> false  false -> true
				st.insert(user_id);
				QJsonObject recObj; 
				QJsonObject filterObj{ {"user_id",user_id} };
				QJsonObject updateObj{ {"isEnable",enable} };
				recObj.insert("filter", filterObj);
				recObj.insert("update", updateObj);
				jArray.append(recObj);

				//直接更新
				m_model->item(i, 6)->setData(enable, Qt::UserRole);
			}
		}
	}
	/*
	Json格式如下：
	{
		lists:[
			{
				filter:{
					"user_id": 123456
				}
				update:{
					"isEnable": true
				}
			},
			{
				filter:{
					"user_id": 555666
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

void UserManagerPage::showUserDetails(const QModelIndex& index)
{
	if (!m_detailsDlg) {
		m_detailsDlg = new UserDetailsDlg(this);
		connect(m_detailsDlg, &UserDetailsDlg::userChanged,this, [=](const QJsonObject& juser) {
			//只能禁用用户
			m_model->item(m_currentIndex.row(), column("isEnable"))->setData(juser.value("isEnable").toBool(), Qt::UserRole);

			sApp->updateUserData(juser);
			});
	}

	m_currentIndex = index; //保存当前详细信息的行索引
	QJsonObject jUser;
	jUser.insert("user_id", m_model->item(index.row(), column("user_id"))->text());
	jUser.insert("user_name", m_model->item(index.row(), column("user_name"))->text());
	jUser.insert("gender", m_model->item(index.row(), column("gender"))->text()=="男" ? 1 : (m_model->item(index.row(), column("gender"))->text()=="女" ? 2 : 0));
	jUser.insert("email", m_model->item(index.row(), column("email"))->text());
	jUser.insert("mobile", m_model->item(index.row(), column("mobile"))->text());
	jUser.insert("isEnable", m_model->item(index.row(), column("isEnable"))->data(Qt::UserRole).toBool());

	m_detailsDlg->resize(this->size());
	m_detailsDlg->setUser(jUser);
	m_detailsDlg->show();
}

void UserManagerPage::showUserEditDlg(const QModelIndex& index)
{
	if (!m_userEditDlg)
	{
		m_userEditDlg = new UserEditDlg(this);
		connect(m_userEditDlg, &UserEditDlg::userChanged,this, [=](const QJsonObject& juser){
			//更新视图
			m_model->item(m_currentIndex.row(), column("user_name"))->setText(juser.value("user_name").toString());
			m_model->item(m_currentIndex.row(), column("email"))->setText(juser.value("email").toString());
			m_model->item(m_currentIndex.row(), column("mobile"))->setText(juser.value("mobile").toString());

			sApp->updateUserData(juser);
			});
	}

	m_currentIndex = index; //保存当前修改的行索引
	QJsonObject jUser;
	jUser.insert("user_id", m_model->item(index.row(), column("user_id"))->text());
	jUser.insert("user_name", m_model->item(index.row(), column("user_name"))->text());
	jUser.insert("email", m_model->item(index.row(), column("email"))->text());
	jUser.insert("mobile", m_model->item(index.row(), column("mobile"))->text());
	jUser.insert("isEnable", m_model->item(index.row(), column("isEnable"))->data(Qt::UserRole).toBool());

	m_userEditDlg->setUser(jUser);
	SMaskWidget::instance()->popup(m_userEditDlg);
}

void UserManagerPage::resizeEvent(QResizeEvent* event)
{
	if (m_detailsDlg) {
		m_detailsDlg->resize(this->size());
	}
}

void UserManagerPage::onBatchEnable()
{
	setBatchEnabeld(true);
}

void UserManagerPage::onBatchDisable()
{
	setBatchEnabeld(false);
}

void UserManagerPage::onBatchDelete()
{
	QJsonObject jObj;
	QJsonArray jArray;
	std::set<int, std::greater<>> deleteRows;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i);
		if (item && item->data(Qt::UserRole).toBool()) { //复选框被选中
			jArray.append(m_model->item(i,column("user_id"))->text()); //user_id
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

void UserManagerPage::onSearch()
{
	QVariantMap params;
	params.insert("isDeleted", false);
	auto filter = ui->searchEdit->text();
	if (!filter.isEmpty()) {
		params.insert("query", filter);
	}
	SHttpClient(URL("/api/user/list")).debug(true)
		.header("Authorization","Bearer" + sApp->userData("user/token").toString())
		.params(params) // 参数形式 ?ddd=&xxx=
		.fail([=](const QString& msg, int code) {
#ifdef _DEBUG
			qWarning()  << msg << code;
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

void UserManagerPage::parseJson(const QJsonObject& obj)
{
	m_model->clear();

	for (int i = 0; i < m_fieldName.size(); i++) {
		//获取user组下的字段名
		m_model->setHorizontalHeaderItem(i, new QStandardItem(SFieldTranslate::instance()->trans("user", m_fieldName[i])));
	}
	auto jArray = obj["data"]["list"].toArray();
	for (int i = 0; i < jArray.size(); i++) {
		auto jUser = jArray[i].toObject();
		//往模型中插入数据
		m_model->appendRow(createItems(jUser));
		
	}
	ui->tableView->setColumnWidth(0, 22);
	ui->tableView->setColumnWidth(1, 100);
	ui->tableView->setColumnWidth(2, 80);
	ui->tableView->setColumnWidth(3, 45);
	ui->tableView->setColumnWidth(4, 100);
	ui->tableView->setColumnWidth(5, 180);
	ui->tableView->setColumnWidth(6, 85);
	ui->tableView->setColumnWidth(7, 300);//TableView_ButtonWidth * 3 + 50
	dynamic_cast<SHeaderView*>(ui->tableView->horizontalHeader())->setState(Qt::Unchecked);
}

int UserManagerPage::column(const QString& field)
{
	auto p = m_fieldName.indexOf(field);
	if (p == -1) {
		qWarning() << "field" << field << "not exists.";
	}
	return p;
}

QList<QStandardItem*> UserManagerPage::createItems(const QJsonObject& jUser)
{
	QList<QStandardItem*> items;
	auto gender = jUser["gender"].toInteger();
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
