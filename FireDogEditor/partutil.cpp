#include "partutil.h"

PartPageInfo PartPageInfo::createPageInfo(int pageIndex, blong pageSize, Part part) {
	blong skip = (pageIndex - 1) * pageSize;
	blong end = part.end + 1;

	blong limit = 0;
	blong pageEnd = skip + pageSize;
	if (pageEnd > end) {
		limit = pageSize + (end - pageEnd);
	}
	else {
		limit = pageSize;
	}
	PartPageInfo partPageInfo;
	partPageInfo.part = part;
	partPageInfo.pageIndex = pageIndex;
	partPageInfo.pageSize  = pageSize;
	partPageInfo.skip = skip;
	partPageInfo.limit = limit;
	return partPageInfo;
}

std::vector<Part> PartUtil::getParts(blong count, int partLength) {
	blong countPartLength = count / partLength;
	std::vector<Part> partsList;
	for (int i = 0; i < partLength; i++) {
		Part part;
		blong nowPartStart = countPartLength * i;
		blong nowPartEnd = 0;
		if (i == partLength - 1) {
			nowPartEnd = count - 1;
		}
		else {
			nowPartEnd = nowPartStart  + countPartLength - 1;
		}
		part.start  = nowPartStart;
		part.end  = nowPartEnd;
		partsList.push_back(part);
	}
	return partsList;
}