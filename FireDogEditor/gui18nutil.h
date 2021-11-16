#ifndef __FIREKYLIN_GUI_I18NUTIL_H__
#define __FIREKYLIN_GUI_I18NUTIL_H__

#include <QGroupBox>
#include <QMenu>
#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

class Gui18nUtil
{
public:
	static QString GetNowLanguage();

	static void SetText(QCheckBox* obj, QString name);
	static void SetText(QRadioButton* obj, QString name);
	static void SetText(QGroupBox* obj,QString name);
	static void SetText(QMenu* obj, QString name);
	static void SetText(QAction* obj, QString name);
	static void SetText(QLabel* obj, QString name);
	static void SetText(QLineEdit* obj, QString name);
	static void SetText(QPushButton* obj, QString name);
	static void SetText(QTabWidget* obj,int index, QString name);
};

#endif