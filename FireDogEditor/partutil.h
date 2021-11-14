#ifndef FIREDOG_PARTUTIL_H
#define FIREDOG_PARTUTIL_H

typedef long long blong;

#include <vector>

/// <summary>
/// 分片
/// </summary>
class Part {
public:
	blong start;
	blong end;
};

class PartPageInfo {
public:
	/**
	 * 页码
	 */
	int pageIndex;

	/**
	 * 每页数量
	 */
	blong pageSize;

	/**
	 * 跳过多少
	 */
	blong skip;

	/**
	 * 长度
	 */
	blong limit;

	/**
	 * 分段
	 */
	Part part;

	static PartPageInfo createPageInfo(int pageIndex, blong pageSize, Part part);
};

class PartUtil {
public:
	static std::vector<Part> getParts(blong count, int partLength);
};

#endif