#ifndef FIREDOG_MATCHTHEAD_H
#define FIREDOG_MATCHTHEAD_H

#include <QThread>
#include <QVector>

/// <summary>
/// ƥ��Ĺ���
/// </summary>
class MatchWork {

};

/// <summary>
/// ƥ���߳�
/// </summary>
class MatchThread : public QThread {

public:
	MatchWork nowWork;
};

/// <summary>
/// �ܼ��߳�
/// </summary>
class MatchButlerThread : public QThread {



};

#endif