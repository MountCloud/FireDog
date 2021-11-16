#ifndef FIREDOG_MATCHTHEAD_H
#define FIREDOG_MATCHTHEAD_H

#include <QThread>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QHash>

#include "partutil.h"

#define WORK_TYPE_TEXT 1
#define WORK_TYPE_HEX 2
#define WORK_TYPE_FILE 3
#define WORK_TYPE_PART 4

#define STATE_NO_ERROR 0
#define STATE_ERROR -1

#define WORK_READ_FILE_LENGTH 1024

namespace firedog {
	class Matcher;
	class FeatureLibrary;
	class FireDog;
}

class HitFeature {
public:
	int workType;
	QString content;

	QString name;
	QString author;
	QString describe;
};

/// <summary>
/// 单个工作
/// </summary>
class MatchWork {
public:
	int workType;
	QString content;
	Part part;
	firedog::Matcher* matcher = NULL;
	~MatchWork();
};

/// <summary>
/// 匹配任务线程
/// </summary>
class MatchWorkThread : public QThread {
	Q_OBJECT;
public:
	MatchWorkThread(int id);
	~MatchWorkThread();

	void run();
	int getState();
	void setWork(MatchWork* work);
private:
	int id;
	int state;
	MatchWork* nowWork = NULL;

	bool match(QByteArray byteArray);

signals:
	void nextWork(MatchWorkThread* misson, int* state);
	void workSuccess(int id);
	void hit(HitFeature hit);
};

/// <summary>
/// 管家线程
/// </summary>
class MatchButlerThread : public QThread {
	Q_OBJECT;
public:
	MatchButlerThread(int threadNum, firedog::FeatureLibrary* featureLibrary);
	~MatchButlerThread();
	int init();

	void run();

	QVector<HitFeature> getHits();
public slots:
	void addWork(MatchWork work);

	void nextWork(MatchWorkThread* misson, int* state);
	void workSuccess(int id);
	void hit(HitFeature hit);
signals:
	void matchEnd(int worksize,int success, int error, int state);
private:
	firedog::FeatureLibrary* featureLibrary = NULL;
	firedog::FireDog* fireDog = NULL;

	int threadNum;
	QHash<int, MatchWorkThread*> workThreads;
	QHash<int,bool> worksStates;
	QQueue<MatchWork> works;
	QMutex workMutex;
	QMutex hitMutex;
	QVector<HitFeature> hits;

};

#endif