#ifndef PERSONALINFO_H
#define PERSONALINFO_
#include <QWidget>
#include <QStandardItemModel>

namespace Ui { class PersonalInfo; }

class PersonalInfo : public QWidget
{
	Q_OBJECT
public:
	PersonalInfo(QWidget* parent = nullptr);
	~PersonalInfo();
	void init();
	void onSearch();
	void onUpload();
private:
	void parseJson(const QJsonObject& obj);
private:
	Ui::PersonalInfo* ui{};

	QStandardItemModel* m_model{};
};

#endif // PERSONALINFO_H