#ifndef __FIREDOG_THREAD_PARSETHREAD_H__
#define __FIREDOG_THREAD_PARSETHREAD_H__

#include <QThread>
#include <QString>

#include "featurelibrary.h"

#define PARSE_STATE_RUNNING 1
#define PARSE_STATE_STOP 0

#define PARSE_SUCCESS 0
#define PARSE_ERROR -1

#define PARSE_SUCCESS 0
#define PARSE_ERROR_CODE_FILE_READ_FAIL -1
#define PARSE_ERROR_CODE_FILE_PARSE_FAIL -2
#define PARSE_ERROR_CODE_FILE_VERSION_CHECK_FAIL -3
#define PARSE_ERROR_CODE_OTHER_FAIL -999

class ParseThread : public QThread {
	Q_OBJECT
public:
	int getState();

	void setFilePath(QString filePath);

private:
	void run() override;
	int state = PARSE_STATE_STOP;

	QString filePath;

signals:
	void parseBegin();
	void parseEnd(firedog::FeatureLibrary* featureLibrary,int state);
};


#endif // !__FIREDOG_GUI_LOADINGDIALOG_H__
