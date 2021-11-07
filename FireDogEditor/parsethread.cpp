#include "parsethread.h"

#include <QFile>

using namespace firedog;

void ParseThread::run() {
	//如果是运行中则不用管
	if (this->state == PARSE_STATE_RUNNING) {
		return;
	}

	emit parseBegin();

	//打开文件并且读取全部内容
	if (this->filePath.isEmpty()) {
		emit parseEnd(NULL, PARSE_ERROR_CODE_FILE_READ_FAIL);
		return;
	}
	QFile file(this->filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		emit parseEnd(NULL, PARSE_ERROR_CODE_FILE_READ_FAIL);
		return;
	}

	QString featureLibraryJson = file.readAll();

	int errorCode = 0;
	FeatureLibrary* library = FeatureLibrary::createByJson(featureLibraryJson.toStdString(),&errorCode);

	int resultCode = PARSE_SUCCESS;
	//有异常的话
	if (library==NULL || errorCode!= NO_ERROR) {
		library = NULL;
		if (errorCode == FL_CONTENT_FORMATE_ERROR) {
			resultCode = PARSE_ERROR_CODE_FILE_PARSE_FAIL;
		}else if (errorCode == FL_CONTENT_VERSION_ERROR) {
			resultCode = PARSE_ERROR_CODE_FILE_VERSION_CHECK_FAIL;
		}
		else {
			resultCode = PARSE_ERROR_CODE_OTHER_FAIL;
		}
	}
	//提交结束
	emit parseEnd(library, resultCode);
}

int ParseThread::getState() {
	return this->state;
}

void ParseThread::setFilePath(QString filePath) {
	this->filePath = filePath;
}