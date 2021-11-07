#include "parsethread.h"

#include <QFile>

using namespace firedog;

void ParseThread::run() {
	//��������������ù�
	if (this->state == PARSE_STATE_RUNNING) {
		return;
	}

	emit parseBegin();

	//���ļ����Ҷ�ȡȫ������
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
	//���쳣�Ļ�
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
	//�ύ����
	emit parseEnd(library, resultCode);
}

int ParseThread::getState() {
	return this->state;
}

void ParseThread::setFilePath(QString filePath) {
	this->filePath = filePath;
}