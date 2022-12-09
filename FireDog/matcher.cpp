#include "matcher.h"

using namespace mountcloud;
using namespace firedog;
using namespace std;


MatcherData::~MatcherData() {

	if (fullData != NULL) {
		AvlMap<char, MatcherData*>::Iterator iter = fullData->begin();
		for (; iter != fullData->end(); iter++) {
			MatcherData* td = iter->mapped_value;
			if (td != NULL) {
				delete td;
				td = NULL;
			}
		}
		fullData->clear();
		delete fullData;
		fullData = NULL;
	}

	if (highPosData != NULL) {
		AvlMap<char, MatcherData*>::Iterator iter = highPosData->begin();
		for (; iter != highPosData->end(); iter++) {
			MatcherData* td = iter->mapped_value;
			if (td != NULL) {
				delete td;
				td = NULL;
			}
		}
		highPosData->clear();
		delete highPosData;
		highPosData = NULL;
	}

	if (lowPosData != NULL) {
		AvlMap<char, MatcherData*>::Iterator iter = lowPosData->begin();
		for (; iter != lowPosData->end(); iter++) {
			MatcherData* td = iter->mapped_value;
			if (td != NULL) {
				delete td;
				td = NULL;
			}
		}
		lowPosData->clear();
		delete lowPosData;
		lowPosData = NULL;
	}

	if (anyData != NULL) {
		delete anyData;
		anyData = NULL;
	}
}

MatcherDataSource::~MatcherDataSource() {
	if (data != NULL) {
		delete data;
		data = NULL;
	}
	if (librarys != NULL) {
		librarys->clear();
		delete librarys;
		librarys = NULL;
	}
	if (libraryItems != NULL) {
		libraryItems->clear();
		delete libraryItems;
		libraryItems = NULL;
	}
	if (matcherFeatures != NULL) {
		for (int i = 0; i < matcherFeatures->size(); i++) {
			MatcherFeature* mf = matcherFeatures->at(i);
			delete mf;
			mf = NULL;
		}
		matcherFeatures->clear();
		delete matcherFeatures;
		matcherFeatures = NULL;
	}
}

MatcherDataSource::MatcherDataSource() {
	librarys = new vector<FeatureLibrary*>();
	libraryItems = new unordered_map<int, FeatureLibraryItem*>();
}

int MatcherDataSource::pushFeatureLibrary(FeatureLibrary* lib) {

	if (lib->items == NULL || lib->items->size() == 0) {
		return M_FEATURE_LIBRARY_IS_EMPTY;
	}

	librarys->push_back(lib);

	for (int i = 0; i < lib->items->size();i++) {
		FeatureLibraryItem* item = lib->items->at(i);
		if (item->features==NULL || item->features->size()==0) {
			continue;
		}

		int featureId = createLibraryId();
		libraryItems->operator[](featureId) = item;
		vector<Feature*>* features = item->features;
		for (int y = 0; y < features->size(); y++) {
			Feature* feature = features->at(y);
			string key = feature->key;

			string hex = feature->hex;
			string text = feature->text;

			key = StringUtil::trim(key);
			hex = StringUtil::trim(hex);
			text = StringUtil::trim(text);

			if (key.size()==0|| (hex.size() == 0 && text.size() == 0)) {
				continue;
			}

			int ec;
			string em;
			HexData* hexData = NULL;

			MatcherFeature* mf = new MatcherFeature();
			mf->featureId = featureId;
			mf->featureKey = key;

			if (this->matcherFeatures == NULL) {
				this->matcherFeatures = new std::vector<MatcherFeature*>();
			}
			this->matcherFeatures->push_back(mf);

			if (text.size()>0) {
				hexData = Converter::textToBytes(text, &ec, &em);
				if (ec == NO_ERROR) {
					if (this->data == NULL) {
						this->data = new MatcherData();
					}
					pushFeature(hexData->hexs, 0, this->data, mf);
					delete hexData;
					hexData = NULL;
				}
			}
			if(hex.size()>0){
				hexData = Converter::hexTextToBytes(hex, &ec, &em);
				if (ec == NO_ERROR) {
					if (this->data == NULL) {
						this->data = new MatcherData();
					}
					pushFeature(hexData->hexs, 0, this->data, mf);
					delete hexData;
					hexData = NULL;
				}
			}

		}
	}
	return NO_ERROR;
}

FeatureLibraryItem* MatcherDataSource::getFeatureLibraryItem(int id) {
	FeatureLibraryItem* item = NULL;
	if (libraryItems != NULL && libraryItems->find(id) != libraryItems->end()) {
		item = libraryItems->at(id);
	}
	return item;
}

void MatcherDataSource::pushFeature(std::vector<std::vector<Hex*>*>* hexs,int index, MatcherData* data, MatcherFeature* mf) {
	
	//笛卡尔积 Cartesian product NxN
	std::vector<Hex*>* shexs = hexs->at(index);
	for (int i = 0; i < shexs->size(); i++) {
		Hex* hex = shexs->at(i);
		int type = hex->type;
		char byte = hex->data;

		MatcherData* tempData = NULL;

		if (type == HEX_DATA_TYPE_FULL) {
			if (data->fullData == NULL) {
				data->fullData = new AvlMap<char, MatcherData*>();
				tempData = new MatcherData();
				data->fullData->insert(byte, tempData);
			}
			else {
				AvlMap<char, MatcherData*>::Iterator ti = data->fullData->find(byte);
				if (ti != NULL) {
					tempData = ti->mapped_value;
				}
				else {
					tempData = new MatcherData();
					data->fullData->insert(byte, tempData);
				}
			}

		}
		else if (type == HEX_DATA_TYPE_HIGH) {
			if (data->highPosData == NULL) {
				data->highPosData = new AvlMap<char, MatcherData*>();
				tempData = new MatcherData();
				data->highPosData->insert(byte, tempData);
			}
			else {
				AvlMap<char, MatcherData*>::Iterator ti = data->highPosData->find(byte);
				if (ti != NULL) {
					tempData = ti->mapped_value;
				}
				else {
					tempData = new MatcherData();
					data->highPosData->insert(byte, tempData);
				}
			}

		}
		else if (type == HEX_DATA_TYPE_LOW) {
			if (data->lowPosData == NULL) {
				data->lowPosData = new AvlMap<char, MatcherData*>();
				tempData = new MatcherData();
				data->lowPosData->insert(byte, tempData);
			}
			else {
				AvlMap<char, MatcherData*>::Iterator ti = data->lowPosData->find(byte);
				if (ti != NULL) {
					tempData = ti->mapped_value;
				}
				else {
					tempData = new MatcherData();
					data->lowPosData->insert(byte, tempData);
				}
			}
		}
		else if (type == HEX_DATA_TYPE_ANY) {
			tempData = data->anyData;
			if (tempData == NULL) {
				tempData = new MatcherData();
				data->anyData = tempData;
			}
		}

		if (index == hexs->size() - 1) {
			if (tempData->features == NULL) {
				tempData->features = new vector<MatcherFeature*>();
			}
			tempData->features->push_back(mf);
		}
		else {
			pushFeature(hexs, index + 1, tempData, mf);
		}
	}
}

int MatcherDataSource::createLibraryId() {
	nowLibraryId = nowLibraryId + 1;
	return nowLibraryId;
}


Matcher::Matcher(MatcherDataSource* dataSource) {
	this->dataSource = dataSource;
}

Matcher::~Matcher() {
	if (matchCache != NULL) {
		//only clear,a MatcherDataSource is used by multiple Matchers.
		//只清空，因为一个MatcherDataSource有多个Matchers在使用
		matchCache->clear();
	}
}


void Matcher::matchBytes(const char* bytes, int length) {
	for (int i = 0; i < length; i++) {
		char byte = bytes[i];
		this->matchByte(byte);
	}
}

void Matcher::matchByte(const char byte) {
	if (matchCache == NULL) {
		matchCache = new std::vector<MatcherData*>();
	}

	//step1: find by cache 先从cache里读取
	if (matchCache->size() > 0) {
		std::vector<MatcherData*>* tempMatchBytesCache = new std::vector<MatcherData*>();
		for (int i = 0; i < this->matchCache->size(); i++) {
			MatcherData* mbd = matchCache->at(i);

			//校验完整字节
			matcherFeature(mbd, byte, tempMatchBytesCache);

		}
		this->matchCache->clear();
		delete this->matchCache;
		this->matchCache = tempMatchBytesCache;
	}

	//step2:find by feature library 然后从特征库中拉取
	//校验完整字节
	matcherFeature(this->dataSource->getData(), byte, this->matchCache);
}

std::vector <MatcherResult*>* Matcher::check() {
	std::vector <MatcherResult*>* result = NULL;

	if (featureRuleData == NULL) {
		return result;
	}
	
	for (std::unordered_map<int, mountcloud::RuleData*>::iterator iter = featureRuleData->begin(); iter != featureRuleData->end(); iter++) {
		int featureId = iter->first;
		mountcloud::RuleData* ruleData = iter->second;
		FeatureLibraryItem* fitem = dataSource->getFeatureLibraryItem(featureId);
		Rule* rule = fitem->rule;
		if (rule != NULL && rule->check(ruleData)) {
			if (result == NULL) {
				result = new std::vector<MatcherResult*>();
			}
			MatcherResult* mr = new MatcherResult();
			mr->featureId = featureId;
			mr->author = fitem->author;
			mr->name = fitem->name;
			mr->describe = fitem->describe;
			result->push_back(mr);
		}
	}

	return result;
}

void Matcher::matcherFeature(MatcherData* mbd, char byte, std::vector<MatcherData*>* tempMatchBytesCache) {
	//高位
	const char highPos = StringUtil::byteHigh(byte);
	//低位
	const char lowPos = StringUtil::byteLow(byte);
	//校验完整字节
	matcherFeatureMap(mbd->fullData, byte, tempMatchBytesCache);

	//校验高位
	matcherFeatureMap(mbd->highPosData, highPos, tempMatchBytesCache);

	//校验低位
	matcherFeatureMap(mbd->lowPosData, lowPos, tempMatchBytesCache);

}

void Matcher::matcherFeatureMap(AvlMap<char, MatcherData*>* map, char byte, std::vector<MatcherData*>* tempMatchBytesCache) {
	if (map==NULL) {
		return;
	}

	AvlMap<char, MatcherData*>::Iterator mbditer = map->find(byte);
	if (mbditer != NULL && mbditer->mapped_value != NULL) {
		MatcherData* iterMbd = mbditer->mapped_value;
		//这里我在想是不是以后可以用多种模式控制，如果是fast模式直接return匹配到的特征？
		if (iterMbd->features != NULL) {
			for (int i = 0; i < iterMbd->features->size(); i++) {
				MatcherFeature* mf = iterMbd->features->at(i);
				int featureId = mf->featureId;
				string featureKey = mf->featureKey;

				RuleData* ruleData = NULL;
				if (featureRuleData == NULL) {
					featureRuleData = new unordered_map<int, mountcloud::RuleData*>();
				}
				if (featureRuleData->find(featureId) == featureRuleData->end()) {
					ruleData = new RuleData();
				}
				else {
					ruleData = featureRuleData->at(featureId);
				}

				ruleData->hit(featureKey);

				featureRuleData->operator[](featureId) = ruleData;

			}
		}
		tempMatchBytesCache->push_back(iterMbd);
	}
}

MatcherData* MatcherDataSource::getData() {
	return data;
}