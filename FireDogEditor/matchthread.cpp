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

void MatchWorkThread::setWork(MatchWork* work) {
	this->nowWork = work;
}

void MatchWorkThread::run() {
	try {
		int nextWorkState = STATE_ERROR;

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
	bool matchstate = false;
	for (int i = 0; i < byteArray.size(); i++) {
		char byte = byteArray.at(i);
		vector<MatcherResult*>* result = matcher->matchByte(byte);
		if (result != NULL) {

			int sourceId = this->nowWork->sourceId;
			int workType = this->nowWork->workType;
			QString workContent = this->nowWork->content;

			for (int i = 0; i < result->size(); i++) {

				MatcherResult* mr = result->at(i);

				HitFeature hf;
				hf.sourceId = sourceId;
				hf.featureId = mr->featureId;
				hf.workType = workType;
				hf.content = workContent;
				hf.name = QString::fromStdString(mr->name);
				hf.author = QString::fromStdString(mr->author);
				hf.describe = QString::fromStdString(mr->describe);

				emit hit(hf);

				delete mr;
				mr = NULL;
			}
			
			delete result;
			result = NULL;
			matchstate =  true;
		}
	}
	return matchstate;
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

void MatchButlerThread::nextWork(MatchWorkThread* workThread, int* state) {
	QMutexLocker locker(&workMutex);
	if (works.isEmpty()) {
		*state = STATE_ERROR;
		return;
	}

	MatchWork tempWork = works.dequeue();

	MatchWork* work = new MatchWork();
	workThread->setWork(work);

	work->sourceId = tempWork.sourceId;
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

	int sourceId = hit.sourceId;
	int featureId = hit.featureId;

	bool canPush = false;
	
	QVector<int> featureIds = this->sourceHitFeatures[sourceId];
	if (!featureIds.contains(featureId)) {
		canPush = true;
		featureIds.push_back(featureId);
		this->sourceHitFeatures[sourceId] = featureIds;
	}

	if (canPush) {
		hits.push_back(hit);
	}
}

int MatchButlerThread::init() {
	fireDog = new FireDog();
	int ecode = fireDog->pushFeatureLibrary(featureLibrary);
	if (ecode != NO_ERROR) {
		return ecode;
	}
	for (int i = 0; i < threadNum; i++) {
		MatchWorkThread* work = new MatchWorkThread(i);
		connect(work, &MatchWorkThread::nextWork, this, &MatchButlerThread::nextWork, Qt::DirectConnection);
		connect(work, &MatchWorkThread::workSuccess, this, &MatchButlerThread::workSuccess);
		connect(work, &MatchWorkThread::hit, this, &MatchButlerThread::hit, Qt::DirectConnection);
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
			this->sleep(1);
		}
	}
}

QVector<HitFeature> MatchButlerThread::getHits() {
	return this->hits;
}

MatchButlerThread::~MatchButlerThread() {
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