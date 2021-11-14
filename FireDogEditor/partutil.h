#ifndef FIREDOG_PARTUTIL_H
#define FIREDOG_PARTUTIL_H

typedef long long blong;

#include <vector>

/// <summary>
/// ��Ƭ
/// </summary>
class Part {
public:
	blong start;
	blong end;
};

class PartPageInfo {
public:
	/**
	 * ҳ��
	 */
	int pageIndex;

	/**
	 * ÿҳ����
	 */
	blong pageSize;

	/**
	 * ��������
	 */
	blong skip;

	/**
	 * ����
	 */
	blong limit;

	/**
	 * �ֶ�
	 */
	Part part;

	static PartPageInfo createPageInfo(int pageIndex, blong pageSize, Part part);
};

class PartUtil {
public:
	static std::vector<Part> getParts(blong count, int partLength);
};

#endif