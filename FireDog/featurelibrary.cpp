#include "featurelibrary.h"

#include "config.h"

using namespace nlohmann;

using namespace firedog;
using namespace std;

int FeatureLibrary::loadByJson(string json) {
	
	nlohmann::json root;
	bool parseStatus = false;
	try {
		root = json::parse(json);
		parseStatus = true;
	}
	catch (nlohmann::detail::parse_error er) {
		parseStatus = false;
		
	}
	if (!parseStatus || !root.is_object()) {
		return FL_CONTENT_FORMATE_ERROR;
	}

	//check content
	if (!root.contains("version") || !root["version"].is_string()
		|| !root.contains("hexItems") || !root["hexItems"].is_array()
		|| !root.contains("md5Items") || !root["md5Items"].is_array()
		|| !root.contains("textItems") || !root["textItems"].is_array()) {
		return FL_CONTENT_FORMATE_ERROR;
	}

	//get version
	string version = root["version"];

	//check version
	if (version != FIREDOG_FEATURE_LIBRARY_VERSION) {
		return FL_CONTENT_VERSION_ERROR;
	}

	
	this->version = version;

	nlohmann::json hexItems = root["hexItems"];
	this->parseJson(&this->hexItems, hexItems);

	nlohmann::json md5Items = root["md5Items"];
	this->parseJson(&this->md5Items, md5Items);

	nlohmann::json textItems = root["textItems"];
	this->parseJson(&this->textItems, textItems);


	return NO_ERROR;
}

FeatureLibrary FeatureLibrary::createByJson(string json, int* errorcode) {
	FeatureLibrary result;
	*errorcode = result.loadByJson(json);
	return result;
}


void FeatureLibrary::parseJson(std::vector<FeatureLibraryItem>* items, nlohmann::json values){

	for (int i = 0; i < values.size(); i++) {
		nlohmann::json value = values[i];
		if (!value.contains("author") || !value["author"].is_string()
			|| !value.contains("createTime") || !value["createTime"].is_string()
			|| !value.contains("content") || !value["content"].is_string()
			|| !value.contains("name") || !value["name"].is_string()
			|| !value.contains("describe") || !value["describe"].is_string()) {
			continue;
		}


		string author = value["author"];
		string createTime = value["createTime"];
		string name = value["name"];
		string describe = value["describe"];
		string content = value["content"];

		FeatureLibraryItem item;
		item.author = author;
		item.createTime = createTime;
		item.name = name;
		item.describe = describe;
		item.content = content;

		items->push_back(item);
	}
}
