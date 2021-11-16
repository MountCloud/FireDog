#include "gui18n.h"

#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qdir.h>
#include <qapplication.h>

Gui18n* Gui18n::instance = nullptr;

Gui18n::Gui18n() {

}

void Gui18n::init() 
{
	this->now_language = GetNowLanguage();

	QString apppath = QApplication::applicationDirPath();
	QString languagePath = apppath + GUI_18N_CONFIG_DIR_NAME + this->now_language + ".json";
	QFile languageFile(languagePath);
	if (languageFile.open(QIODevice::ReadOnly)) {
		QByteArray allData = languageFile.readAll();
		languageFile.close();

		QJsonParseError jsonError;
		QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

		if (jsonError.error == QJsonParseError::NoError)
		{
			QJsonObject rootObj = jsonDoc.object();
			QStringList qlist = rootObj.keys();
			for (int i = 0; i < qlist.size(); i++) {
				QString key = qlist[i];
				QString value = rootObj.value(key).toString();
				this->configs[key] = value;
			}
		}
		else {
			qDebug() << "language parse json fail." << jsonError.errorString();
		}
	}
	else {
		qDebug() << "language open file fail.";
	}
}

Gui18n* Gui18n::GetInstance() {
	if (instance == nullptr) {
		instance = new Gui18n();
		instance->init();
	}
	return instance;
}

QString Gui18n::GetConfig(QString name) {
	return this->GetConfig(name,"");
}

QString Gui18n::GetConfig(QString name,QString defstr) {
	if (this->configs.find(name) != this->configs.end()) {
		return this->configs[name];
	}
	return defstr;
}

QString Gui18n::GetNowLanguage() {
	QString apppath = QApplication::applicationDirPath();
	QString languageConfigPath = apppath + "/" + GUI_18N_CONFIG_FILE_NAME;

	QFile languageConfigFile(languageConfigPath);
	if (!languageConfigFile.exists()) {

		if (languageConfigFile.open(QIODevice::WriteOnly)) {
			QJsonObject config;
			config["language"] = "en-us";
			QJsonDocument document;
			document.setObject(config);
			languageConfigFile.write(document.toJson());
			languageConfigFile.flush();
			languageConfigFile.close();
		}
		else {
			qDebug() << "language config not exists,open file fail.";
		}
		return GUI_18N_DEFAULT_LANGUAGE;
	}
	else {
		if (languageConfigFile.open(QIODevice::ReadOnly)) {
			QByteArray allData = languageConfigFile.readAll();
			languageConfigFile.close();

			QJsonParseError jsonError;
			QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

			if (jsonError.error != QJsonParseError::NoError)
			{
				qDebug() << "json error!" << jsonError.errorString();
				this->now_language = GUI_18N_DEFAULT_LANGUAGE;
				return this->now_language;
			}

			QJsonObject rootObj = jsonDoc.object();
			QString language = rootObj.value("language").toString();
			if (language.isEmpty()) {
				SetLanguage(GUI_18N_DEFAULT_LANGUAGE);
			}
			else {
				return language;
			}
		}
		else {
			qDebug() << "language config open file fail.";
			return GUI_18N_DEFAULT_LANGUAGE;
		}
	}
	return GUI_18N_DEFAULT_LANGUAGE;
}

void Gui18n::SetLanguage(QString language) {
	if (language==this->now_language) {
		return;
	}
	this->now_language = language;
	this->configs.clear();

	QString apppath = QApplication::applicationDirPath();
	QString languageConfigPath = apppath + "/" + GUI_18N_CONFIG_FILE_NAME;
	QFile languageConfigFile(languageConfigPath);
	if (languageConfigFile.open(QIODevice::WriteOnly)) {
		QJsonObject config;
		config["language"] = language;
		QJsonDocument document;
		document.setObject(config);
		languageConfigFile.write(document.toJson());
		languageConfigFile.flush();
		languageConfigFile.close();
	}

	instance->init();
}

QVector<QString> Gui18n::GetLanguages() {
	QVector<QString> result;

	QString apppath = QApplication::applicationDirPath();
	QString languageDirPath = apppath + GUI_18N_CONFIG_DIR_NAME;
	QDir languageDir(languageDirPath);

	if (languageDir.exists()) {
		QFileInfoList fileInfoListInSplDir = languageDir.entryInfoList(QDir::Files);
		QFileInfo tempFileInfo;
		foreach(tempFileInfo, fileInfoListInSplDir) {
			// absoluteFilePath()   -- 截取到的是文件夹的路径
			// absolutePath()       -- 截取的是文件夹所在路径
			//filePathNames << tempFileInfo.absoluteFilePath();
			QString filename = tempFileInfo.fileName();
			if (filename.endsWith(".json")) {
				QString basefilename = tempFileInfo.baseName();
				result.push_back(basefilename);
			}
		}
	}

	return result;
}