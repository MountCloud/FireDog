#include "featurelibrary.h"

#include "json/json.h"
#include "config.h"

using namespace firedog;
using namespace std;

int FeatureLibrary::loadByJson(string json) {
	Json::CharReaderBuilder rbuilder;
	rbuilder["collectComments"] = false;

	string errs;
	stringstream ss;
	ss << json;

	Json::Value root;
	bool ok = Json::parseFromStream(rbuilder, ss, &root, &errs);

	if (!ok || errs.size() > 0 || !root.isObject()) {
		return FL_CONTENT_FORMATE_ERROR;
	}

	//check content
	if (!root.isMember("version") || !root["version"].isString()
		|| !root.isMember("author") || !root["author"].isString()
		|| !root.isMember("createTime") || !root["createTime"].isString()
		|| !root.isMember("hexItems") || !root["hexItems"].isArray()
		|| !root.isMember("md5Items") || !root["md5Items"].isArray()
		|| !root.isMember("textItems") || !root["textItems"].isArray()) {
		return FL_CONTENT_FORMATE_ERROR;
	}

	//get version
	string version = root["version"].asString();

	//check version
	if (version != FIREDOG_FEATURE_LIBRARY_VERSION) {
		return FL_CONTENT_VERSION_ERROR;
	}

	
	this->version = version;

	this->author = root["author"].asString();
	this->createTime = root["createTime"].asString();

	Json::Value hexItems = root["hexItems"];
	this->parseJson(&this->hexItems, hexItems);

	Json::Value md5Items = root["md5Items"];
	this->parseJson(&this->md5Items, md5Items);

	Json::Value textItems = root["textItems"];
	this->parseJson(&this->textItems, textItems);


	return NO_ERROR;
}

FeatureLibrary FeatureLibrary::createByJson(string json, int* errorcode) {
	FeatureLibrary result;
	*errorcode = result.loadByJson(json);
	return result;
}


void FeatureLibrary::parseJson(std::vector<FeatureLibraryItem>* items, Json::Value values){

	for (int i = 0; i < values.size(); i++) {
		Json::Value value = values[i];
		if (!value.isMember("content") || !value["content"].isString()
			|| !value.isMember("name") || !value["name"].isString()
			|| !value.isMember("describe") || !value["describe"].isString()) {
			continue;
		}

		string name = value["name"].asString();
		string describe = value["describe"].asString();
		string content = value["content"].asString();

		FeatureLibraryItem item;
		item.name = name;
		item.describe = describe;
		item.content = content;

		items->push_back(item);
	}
}
