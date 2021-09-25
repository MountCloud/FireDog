#include "matcher.h"

#include "featurelibrary.h"
#include "stringutil.h"

using namespace firedog;
using namespace std;

MatcherFeature::~MatcherFeature() {
	name.clear();
	author.clear();
	describe.clear();
	content.clear();
}

MatcherMd5Data::~MatcherMd5Data() {
	if (data != NULL) {
		std::unordered_map<std::string, MatcherFeature*>::iterator iter;
		for (iter = data->begin(); iter != data->end(); ++iter) {
			MatcherFeature* td = iter->second;
			if (td != NULL) {
				delete td;
				td = NULL;
			}
		}
		data->clear();
		delete data;
		data = NULL;
	}
}

MatcherByteData::~MatcherByteData() {

	if (feature != NULL) {
		delete feature;
		feature = NULL;
	}

	if (data != NULL) {
		AvlMap<char, MatcherByteData*>::Iterator iter = data->begin();
		for (; iter != data->end(); iter++) {
			MatcherByteData* td = iter->mapped_value;
			if (td != NULL) {
				delete td;
				td = NULL;
			}
		}
		data->clear();
		delete data;
		data = NULL;
	}
}

MatcherDataSource::~MatcherDataSource() {
	if (md5Data!=NULL) {
		delete md5Data;
		md5Data = NULL;
	}
	if (byteData != NULL) {
		delete byteData;
		byteData = NULL;
	}
	if (librarys != NULL) {
		librarys->clear();
		delete librarys;
		librarys = NULL;
	}
}

MatcherDataSource::MatcherDataSource() {
	md5Data = new MatcherMd5Data();
	md5Data->data = new std::unordered_map<std::string, MatcherFeature*>();

	byteData = new MatcherByteData();
	byteData->data = new AvlMap<char, MatcherByteData*>();

	librarys = new vector<FeatureLibrary>();
}

int MatcherDataSource::pushFeatureLibrary(FeatureLibrary lib) {

	//if (lib.hexItems.size() == 0 && lib.md5Items.size() == 0 && lib.textItems.size() == 0) {
	//	return M_FEATURE_LIBRARY_IS_EMPTY;
	//}

	////push md5
	//if (lib.md5Items.size() > 0) {
	//	for (int i = 0; i < lib.md5Items.size(); i++) {
	//		FeatureLibraryItem item = lib.md5Items[i];
	//		MatcherFeature* mf = toMatcherFeature(item, FEATURE_TYPE_MD5);
	//		//md5 key 
	//		string key = item.content;
	//		md5Data->data->emplace(key, mf);
	//	}
	//}

	////push hex
	//if (lib.hexItems.size() > 0) {
	//	for (int i = 0; i < lib.hexItems.size(); i++) {
	//		FeatureLibraryItem item = lib.hexItems[i];
	//		MatcherFeature* mf = toMatcherFeature(item, FEATURE_TYPE_MD5);
	//		//hex str
	//		string hexstr = item.content;
	//		//bytes
	//		vector<char> bytes;
	//		StringUtil::hexTextToBytes(hexstr, &bytes);
	//		pushByteFeature(&bytes, mf);
	//	}
	//}

	////push text
	//if (lib.textItems.size() > 0) {
	//	for (int i = 0; i < lib.textItems.size(); i++) {
	//		FeatureLibraryItem item = lib.textItems[i];
	//		MatcherFeature* mf = toMatcherFeature(item, FEATURE_TYPE_TEXT);
	//		//text
	//		string text = item.content;
	//		//bytes
	//		vector<char> bytes;
	//		StringUtil::textToBytes(text, &bytes);
	//		pushByteFeature(&bytes, mf);
	//	}
	//}

	return NO_ERROR;
}

void MatcherDataSource::pushByteFeature(std::vector<char>* bytes, MatcherFeature* mf) {
	MatcherByteData* tempData = this->byteData;
	for (int i = 0; i < bytes->size(); i++) {
		char byte = bytes->at(i);

		AvlMap<char, MatcherByteData*>::Iterator ti = tempData->data->find(byte);

		//last byte 如果是最后字节，将特征给data
		if (i == bytes->size() - 1) {
			if (ti != NULL) {
				MatcherByteData* td = ti->mapped_value;
				td->feature = mf;
			}else{
				MatcherByteData* data = new MatcherByteData();
				data->feature = mf;
				tempData->data->insert(byte, data);
			}
		}
		//not last byte不是最后的字节，构建多重树
		else {
			if (ti != NULL) {
				tempData = ti->mapped_value;
			}else{
				MatcherByteData* data = new MatcherByteData();
				data->data = new AvlMap<char, MatcherByteData*>();

				tempData->data->insert(byte, data);
				tempData = data;
			}
		}
	}
}

MatcherFeature* MatcherDataSource::toMatcherFeature(FeatureLibraryItem item, int type) {
	MatcherFeature* mf = new MatcherFeature();
	mf->name = item.name;
	mf->author = item.author;
	mf->describe = item.describe;
	//mf->content = item.content;
	mf->type = type;

	return mf;
}

Matcher::Matcher(MatcherDataSource* dataSource) {
	this->dataSource = dataSource;
}

Matcher::~Matcher() {
	if (matchBytesCache != NULL) {
		//only clear,a MatcherDataSource is used by multiple Matchers.
		//只清空，因为一个MatcherDataSource有多个Matchers在使用
		matchBytesCache->clear();
	}
}

MatcherFeature* Matcher::matchMd5(string md5) {
	MatcherFeature* mf = NULL;

	try {
		mf = this->dataSource->md5Data->data->at(md5);
	}
	catch (std::out_of_range ex) {
		//not found
		mf = NULL;
	}

	return mf;
}

MatcherFeature* Matcher::matchBytes(const char* bytes, int length) {
	MatcherFeature* mf = NULL;
	for (int i = 0; i < length; i++) {
		char byte = bytes[i];
		mf = this->matchByte(byte);
		if (mf != NULL) {
			break;
		}
	}
	return mf;
}

MatcherFeature* Matcher::matchByte(const char byte) {
	MatcherFeature* mf = NULL;

	if (matchBytesCache == NULL) {
		matchBytesCache = new std::vector<MatcherByteData*>();
	}
	//step1: find by cache 先从cache里读取
	if (matchBytesCache->size() > 0) {
		std::vector<MatcherByteData*>* tempMatchBytesCache = new std::vector<MatcherByteData*>();
		for (int i = 0; i < this->matchBytesCache->size(); i++) {
			MatcherByteData* mbd = matchBytesCache->at(i);
			AvlMap<char, MatcherByteData*>::Iterator mbditer = mbd->data->find(byte);
			if (mbditer != NULL&& mbditer->mapped_value!=NULL) {
				MatcherByteData* iterMbd = mbditer->mapped_value;
				//这里我在想是不是以后可以用多种模式控制，如果是fast模式直接return匹配到的特征？
				if (iterMbd->feature != NULL) {
					mf = iterMbd->feature;
				}
				tempMatchBytesCache->push_back(iterMbd);
			}
		}
		this->matchBytesCache->clear();
		delete this->matchBytesCache;
		this->matchBytesCache = tempMatchBytesCache;
	}

	//step2:find by feature library 然后从特征库中拉取
	AvlMap<char, MatcherByteData*>::Iterator iter = this->dataSource->byteData->data->find(byte);
	MatcherByteData* findData = NULL;
	if (iter != NULL) {
		findData = iter->mapped_value;
		if (findData->feature != NULL) {
			mf = findData->feature;
		}
		this->matchBytesCache->push_back(findData);
	}

	return mf;
}