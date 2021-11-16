#include "gui18nutil.h"

#include "gui18n.h"

QString Gui18nUtil::GetNowLanguage() {
	return Gui18n::GetInstance()->GetNowLanguage();
}

void Gui18nUtil::SetText(QCheckBox* obj, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setText(text);
	}
}

void Gui18nUtil::SetText(QRadioButton* obj, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setText(text);
	}
}

void Gui18nUtil::SetText(QGroupBox* obj, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setTitle(text);
	}
}

void Gui18nUtil::SetText(QMenu* obj, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setTitle(text);
	}
}

void Gui18nUtil::SetText(QAction* obj, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setText(text);
	}
}

void Gui18nUtil::SetText(QLabel* obj, QString name)
{
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setText(text);
	}
}

void Gui18nUtil::SetText(QLineEdit* obj, QString name)
{
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setPlaceholderText(text);
	}
}

void Gui18nUtil::SetText(QPushButton* obj, QString name)
{
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setText(text);
	}
}

void Gui18nUtil::SetText(QTabWidget* obj,int index, QString name) {
	Gui18n* gui18n = Gui18n::GetInstance();

	QString text = gui18n->GetConfig(name);
	if (!text.isEmpty()) {
		obj->setTabText(index, text);
	}
}