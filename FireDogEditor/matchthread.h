#ifndef FIREDOG_MATCHTHEAD_H
#define FIREDOG_MATCHTHEAD_H

#include <QThread>
#include <QVector>

/// <summary>
/// 匹配的工作
/// </summary>
class MatchWork {

};

/// <summary>
/// 匹配线程
/// </summary>
class MatchThread : public QThread {

public:
	MatchWork nowWork;
};

/// <summary>
/// 管家线程
/// </summary>
class MatchButlerThread : public QThread {



};

#endif