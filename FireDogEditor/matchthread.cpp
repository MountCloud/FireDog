#include "matchthread.h"

#include <QFile>
#include <QDataStream>

#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"
#include "stringutil.h"

using namespace firedog;

MatchWork::~MatchWork() {
	if (matcher != NULL) {
		delete matcher;
		matcher = NULL;
	}
}

MatchWorkThread::MatchWorkThread(int id) {
	this->id = id;
}

void MatchWorkThread::run() {
	try {
		int nextWorkState = STATE_NO_ERROR;

		while (true) {
			if (this->nowWork != NULL) {
				delete this->nowWork;
				this->nowWork = NULL;
			}
			emit nextWork(this, &nextWorkState);
			if (nextWorkState != STATE_NO_ERROR) {
				break;
			}

			//开始执行任务
			int workType = this->nowWork->workType;
			QString workContent = this->nowWork->content;
			Part part = this->nowWork->part;
			//根据类型来进行匹配
			//文本类型
			if (workType == WORK_TYPE_TEXT) {
				QByteArray bytes = workContent.toLocal8Bit();
				match(bytes);
			}
			//hex类型
			else if (workType == WORK_TYPE_HEX) {
				std::string contentStr = workContent.toStdString();
				std::vector<char> hexBytes;
				StringUtil::hexTextToBytes(contentStr, &hexBytes);

				QByteArray bytes = QByteArray(reinterpret_cast<const char*>(hexBytes.data()), hexBytes.size());
				match(bytes);
			}
			//单文件类型
			else if (workType == WORK_TYPE_FILE) {
				QFile file(workContent);
				if (!file.open(QIODevice::ReadOnly)) {
					this->state = STATE_ERROR;
					continue;
				}
				while (true)
				{
					QByteArray bytes = file.read(WORK_READ_FILE_LENGTH);
					bool matchState = match(bytes);
					//匹配到了或者文件读完了
					if (bytes.size() < WORK_READ_FILE_LENGTH || matchState) {
						break;
					}
				}
				file.close();
			}
			//分片文件类型
			else if (workType == WORK_TYPE_PART) {
				QFile file(workContent);
				if (!file.open(QIODevice::ReadOnly)) {
					this->state = STATE_ERROR;
					continue;
				}
				blong start = part.start;
				blong end = part.end;
				//跳过
				file.skip(start);

				blong readlength = start;
				while (true)
				{
					//计算结束
					int len = WORK_READ_FILE_LENGTH;
					if (readlength + len > end) {
						len = WORK_READ_FILE_LENGTH - (readlength + len - end);
					}
					QByteArray bytes = file.read(WORK_READ_FILE_LENGTH);
					readlength = readlength + len;

					bool matchState = match(bytes);
					//匹配到了或者文件读完了
					if (bytes.size() < WORK_READ_FILE_LENGTH || matchState) {
						break;
					}
				}
			}
		}
		
		this->state = STATE_NO_ERROR;
	}
	catch (...) {
		this->state = STATE_ERROR;
	}
	emit workSuccess(this->id);
}

bool MatchWorkThread::match(QByteArray byteArray) {
	Matcher* matcher = this->nowWork->matcher;
	for (int i = 0; i < byteArray.size(); i++) {
		char byte = byteArray.at(i);
		MatcherResult* result = matcher->matchByte(byte);
		if (result != NULL) {
			int workType = this->nowWork->workType;
			QString workContent = this->nowWork->content;

			HitFeature hf;
			hf.workType = workType;
			hf.content = workContent;
			hf.name = QString::fromStdString(result->name);
			hf.author = QString::fromStdString(result->author);
			hf.describe = QString::fromStdString(result->describe);

			delete result;
			result = NULL;

			emit hit(hf);
			return true;
		}
	}
	return false;
}

int MatchWorkThread::getState() {
	return this->state;
}

MatchButlerThread::MatchButlerThread(int threadNum, firedog::FeatureLibrary* featureLibrary) {
	this->threadNum = threadNum;
	this->featureLibrary = featureLibrary;
}

MatchWorkThread::~MatchWorkThread() {
	if (nowWork!=NULL) {
		delete nowWork;
		nowWork = NULL;
	}
}

void MatchButlerThread::nextWork(MatchWorkThread* misson, int* state) {
	QMutexLocker locker(&workMutex);
	if (works.isEmpty()) {
		*state = STATE_ERROR;
		return;
	}

	MatchWork tempWork = works.dequeue();

	MatchWork* work = new MatchWork();
	misson->setWork(work);

	work->workType = tempWork.workType;
	work->content = tempWork.content;
	work->part = tempWork.part;
	work->matcher = fireDog->createNewMatcher();

	*state = STATE_NO_ERROR;
}

void MatchButlerThread::addWork(MatchWork work) {
	QMutexLocker locker(&workMutex);
	works.enqueue(work);
}

void MatchButlerThread::workSuccess(int id) {
	worksStates.insert(id, true);
}

void MatchButlerThread::hit(HitFeature hit) {
	QMutexLocker locker(&hitMutex);
	hits.push_back(hit);
}

int MatchButlerThread::init() {
	FireDog* fireDog = new FireDog();
	int ecode = fireDog->pushFeatureLibrary(featureLibrary);
	if (ecode != NO_ERROR) {
		return ecode;
	}
	for (int i = 0; i < threadNum; i++) {
		MatchWorkThread* work = new MatchWorkThread(i);
		connect(work, &MatchWorkThread::nextWork, this, &MatchButlerThread::nextWork);
		connect(work, &MatchWorkThread::workSuccess, this, &MatchButlerThread::workSuccess);
		connect(work, &MatchWorkThread::hit, this, &MatchButlerThread::hit);
		workThreads.insert(i, work);
		worksStates.insert(i, false);
	}
	return STATE_NO_ERROR;
}

void MatchButlerThread::run() {
	int initState = init();
	if (initState != STATE_NO_ERROR) {
		emit matchEnd(0,0,0,initState);
		return;
	}
	//start work threads
	QHash<int, MatchWorkThread*>::const_iterator it;
	for (it = workThreads.constBegin(); it != workThreads.constEnd(); it++) {
		MatchWorkThread* thread = it.value();
		thread->start();
	}

	while (true)
	{
		int success = 0;
		int error = 0;

		bool isEnd = true;
		bool existSuccess = false;
		for (int i = 0; i < threadNum; i++) {
			bool workState = worksStates.value(i);
			if (!workState) {
				isEnd = false;
			}
			else {
				MatchWorkThread* thread = workThreads.value(i);
				if (thread->getState() == STATE_NO_ERROR) {
					existSuccess = true;
					success = success + 1;
				}
				else {
					error = error + 1;
				}
			}
		}
		if (isEnd) {
			int matchState = existSuccess ? STATE_NO_ERROR : STATE_ERROR;
			emit matchEnd(threadNum, success, error, matchState);
			break;
		}
		else {
			this->sleep(3);
		}
	}
}

MatchButlerThread::~MatchButlerThread() {
	if (featureLibrary != NULL) {
		delete featureLibrary;
		featureLibrary = NULL;
	}
	if (fireDog != NULL) {
		delete fireDog;
		fireDog = NULL;
	}
	if (!workThreads.isEmpty()) {
		QHash<int, MatchWorkThread*>::const_iterator it;
		for (it = workThreads.constBegin(); it != workThreads.constEnd(); it++) {
			MatchWorkThread* thread = it.value();
			delete thread;
		}
		workThreads.clear();
	}
	if (!works.isEmpty()) {
		works.clear();
	}
}