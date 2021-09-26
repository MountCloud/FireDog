#include "featurelibrary.h"

#include "config.h"
#include "json/json-schema/json-schema.hpp"

#include "rule/rule.h"

using namespace nlohmann;
using nlohmann::json_schema::json_validator;

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

	bool validstate = false;
	auto schema = json::parse(FIREDOG_FEATURE_LIBRARAY_JSON_SCHEMA);

	json_validator validator; // create validator
	validator.set_root_schema(schema);
	try {
		validator.validate(root);
		validstate = true;
	}
	catch (const std::exception& e) {
		validstate = false;
	}
	
	if (!validstate) {
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
				&& item.contains("rule") && item["rule"].is_object()) {
				
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
				for (int y = 0; y < features.size(); y++) {
					nlohmann::json feature = features[y];
					if (feature.contains("key") && feature["key"].is_string()
						&& (feature.contains("hex") && feature["hex"].is_string()
							|| feature.contains("text") && feature["text"].is_string())) {
						string fkey = feature["key"];
						string fhex = "";
						string ftext = "";
						if (feature.contains("hex")) {
							fhex = feature["hex"];
						}

						if (feature.contains("text")) {
							ftext = feature["text"];
						}

						Feature* feature = new Feature();
						feature->key = fkey;
						feature->hex = fhex;
						feature->text = ftext;
						fitem->features->push_back(feature);
					}
				}
				result->items->push_back(fitem);
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
			for (int i = 0; i < andr.size(); i++) {
				nlohmann::json tandr = andr[i];
				if (tandr.is_string()) {
					string tid = tandr;
					Rule* andids = new Rule();
					andids->id = tid;
					rule->ands->push_back(andids);
				}
			}
		}
	}

	if (rulejson.contains("$or") && rulejson["$or"].is_array() && rulejson["$or"].size() > 0) {
		nlohmann::json orr = rulejson["$or"];
		nlohmann::json orrone = orr[0];
		rule->ors = new vector<Rule*>();
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
			for (int i = 0; i < orr.size(); i++) {
				nlohmann::json torr = orr[i];
				if (torr.is_string()) {
					string tid = torr;

					Rule* orids = new Rule();
					orids->id = tid;
					rule->ors->push_back(orids);
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
	if (rule != NULL) {
		delete rule;
		rule = NULL;
	}
}