#include "featurelibrary.h"

#include "config.h"
#include "json/json-schema/json-schema.hpp"

#include "rule/rule.h"

using namespace nlohmann;

using namespace mountcloud;
using namespace firedog;
using namespace std;


FeatureLibrary::FeatureLibrary() {
	items = new std::vector<FeatureLibraryItem*>();
}

FeatureLibrary::~FeatureLibrary() {
	if (items != NULL) {
		for (int i = 0; i < items->size(); i++) {
			FeatureLibraryItem* item = items->at(i);
			delete item;
			item = NULL;
		}
	}
}

FeatureLibrary* FeatureLibrary::createByJson(string json, int* errorcode) {
	FeatureLibrary* result = NULL;
	nlohmann::json root;
	bool parseStatus = false;
	try {
		root = json::parse(json);
		parseStatus = true;
	}
	catch (nlohmann::detail::parse_error er) {
		parseStatus = false;

	}
	if (!parseStatus || !root.is_object()|| !root.contains("version") || !root.contains("items")) {
		*errorcode = FL_CONTENT_FORMATE_ERROR;
		return result;
	}

	
	//get version
	string version = root["version"];

	//check version
	if (version != FIREDOG_FEATURE_LIBRARY_VERSION) {
		*errorcode = FL_CONTENT_VERSION_ERROR;
	}

	result = new FeatureLibrary();

	result->version = version;

	nlohmann::json items = root["items"];

	if (items.is_array()) {
		for (int i = 0; i < items.size(); i++) {
			nlohmann::json item = items[i];
			if (item.is_object()
				&& item.contains("author") && item["author"].is_string()
				&& item.contains("name") && item["name"].is_string()
				&& item.contains("describe") && item["describe"].is_string()
				&& item.contains("features") && item["features"].is_array() && item["features"].size() > 0
				&& item.contains("role") && item["role"].is_object()) {
				
				//先看规则
				nlohmann::json rulejson = item["rule"];

				Rule* rule = parseRule(rulejson);
				if (rule == NULL) {
					continue;
				}

				FeatureLibraryItem* fitem = new FeatureLibraryItem();
				fitem->author = item["author"];
				fitem->name = item["name"];
				fitem->describe = item["describe"];
				fitem->rule = rule;

				nlohmann::json features = item["features"];
				for (int i = 0; i < features.size(); i++) {
					nlohmann::json feature = features[i];
					if (feature.contains("name") && feature["name"].is_string()
						&& feature.contains("type") && feature["type"].is_string()
						&& feature.contains("content") && feature["content"].is_string()) {
						string fname = feature["name"];
						string ftype = feature["type"];
						string fcontent = feature["content"];
						Feature* feature = new Feature();
						feature->name = fname;
						feature->type = ftype;
						feature->content = fcontent;
						fitem->features->push_back(feature);
					}
				}
			}
		}
	}

	return result;
}

Rule* FeatureLibrary::parseRule(nlohmann::json rulejson) {
	if (!rulejson.is_object()) {
		return NULL;
	}
	Rule* rule = new Rule();

	if (rulejson.contains("$and")&&rulejson["$and"].is_array()&& rulejson["$and"].size()>0) {
		nlohmann::json andr = rulejson["$and"];
		nlohmann::json androne = andr[0];
		rule->ands = new vector<Rule*>();
		//说明是组合
		if (androne.is_object()) {
			for (int i = 0; i < andr.size(); i++) {
				nlohmann::json tandr = andr[i];
				Rule* tr = parseRule(tandr);
				if (tr==NULL) {
					continue;
				}
				rule->ands->push_back(tr);
			}
		}
		//说明是id咯
		else {
			Rule* andids = new Rule();
			andids->ids = new vector<string>();
			for (int i = 0; i < andr.size(); i++) {
				nlohmann::json tandr = andr[i];
				if (tandr.is_string()&& tandr.size()>0) {
					string tid = tandr;
					andids->ids->push_back(tid);
				}
			}
		}
	}

	if (rulejson.contains("$or") && rulejson["$or"].is_array() && rulejson["$or"].size() > 0) {
		nlohmann::json orr = rulejson["$or"];
		nlohmann::json orrone = orr[0];
		rule->-> = new vector<Rule*>();
		//说明是组合
		if (orrone.is_object()) {
			for (int i = 0; i < orr.size(); i++) {
				nlohmann::json torr = orr[i];
				Rule* tr = parseRule(torr);
				if (tr == NULL) {
					continue;
				}
				rule->ors->push_back(tr);
			}
		}
		//说明是id咯
		else {
			Rule* andids = new Rule();
			andids->ids = new vector<string>();
			for (int i = 0; i < orr.size(); i++) {
				nlohmann::json torr = orr[i];
				if (orr.is_string() && torr.size() > 0) {
					string tid = torr;
					andids->ids->push_back(tid);
				}
			}
		}
	}

	return rule;
}


FeatureLibraryItem::FeatureLibraryItem() {
	features = new vector<Feature*>();
}

FeatureLibraryItem::~FeatureLibraryItem() {
	if (features != NULL) {
		for (int i = 0; i < features->size(); i++) {
			Feature* content = features->at(i);
			delete content;
			content = NULL;
		}
	}
}