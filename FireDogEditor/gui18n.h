#ifndef __FIREKYLIN_GUI_I18N_H__
#define __FIREKYLIN_GUI_I18N_H__
#include <QHash>
#include <qstring.h>
#include <qvector.h>

#define GUI_18N_CONFIG_DIR_NAME "/fdlanguages/"
#define GUI_18N_CONFIG_FILE_NAME "firedog.i18n.config.json"
#define GUI_18N_DEFAULT_LANGUAGE "en-us"

class Gui18n
{
public:
	static Gui18n* GetInstance();
	QString GetNowLanguage();
	//À˘”–µƒ”Ô—‘
	QVector<QString> GetLanguages();
	QString GetConfig(QString name);
	QString GetConfig(QString name, QString defstr);
	void SetLanguage(QString language);
private:
	QString now_language;
	QHash<QString, QString> configs;
	Gui18n();
	static Gui18n* instance;
	void init();
};
#endif