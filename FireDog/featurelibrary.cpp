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

				vector<Rule*> rules = parseRules(rulejson);
				if (rules.size()==0) {
					continue;
				}

				Rule* rule = NULL;
				if (rules.size()==1) {
					rule = rules.at(0);
				}
				else {
					AndRule* andRule = new AndRule();
					andRule->addRules(rules);
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

vector<Rule*> FeatureLibrary::parseRules(nlohmann::json rulejson) {
	vector<Rule*> rules;

	auto eachlogicchild = [](nlohmann::json childjson, LogicRule* logicRule) {
		for (int i = 0; i < childjson.size(); i++) {
			nlohmann::json cj = childjson[i];
			vector<Rule*> rules = parseRules(cj);
			if (rules.size() > 0) {
				logicRule->addRules(rules);
			}
		}
	};
	if (rulejson.is_string()) {
		string id = rulejson;
		CountRule* countRule = new CountRule(id);
		rules.push_back(countRule);
	}else if (rulejson.is_object()&&!rulejson.is_array()) {
		for (auto ruleiter = rulejson.begin(); ruleiter != rulejson.end(); ruleiter++) {
			std::string key = ruleiter.key();
			nlohmann::json childrulejson = rulejson[key];
			if (key == "$and") {
				AndRule* andRule = new AndRule();
				eachlogicchild(childrulejson, andRule);
				rules.push_back(andRule);
			}
			else if(key == "$or") {
				OrRule* orRule = new OrRule();
				eachlogicchild(childrulejson, orRule);
				rules.push_back(orRule);
			}
			else if (key == "$not") {
				NotRule* notRule = new NotRule();
				eachlogicchild(childrulejson, notRule);
				rules.push_back(notRule);
			}
			else if (key == "$int") {
				if (childrulejson.is_number()) {
					long num = childrulejson;
					IntRule* intRule = new IntRule(childrulejson);
					rules.push_back(intRule);
				}
			}
			else if (key == "$count") {
				if (childrulejson.is_array()) {
					vector<string> ids;
					long num = 1L;
					int idslen = childrulejson.size();
					for (int i = 0; i < idslen; i++) {
						nlohmann::json crj = childrulejson[i];
						if (crj.is_string()) {
							string id = crj;
							ids.push_back(id);
						}
						else if (i == idslen - 1 && crj.is_number()) {
							long tempnum = crj;
							if (tempnum > 1) {
								num = tempnum;
							}
						}
					}
					if (ids.size() > 0) {
						CountRule* countRule = new CountRule(ids);
						countRule->setCritical(num);
						rules.push_back(countRule);
					}
				}
			}
			else if(key == "$lt"|| key == "$le" || key == "$gt" || key == "$ge") {
				if (!childrulejson.is_array()&&childrulejson.size()==2) {
					Rule* compareRule = NULL;
					nlohmann::json num1json = childrulejson[0];
					nlohmann::json num2json = childrulejson[1];
					Rule* num1Rule = parseRule(num1json);
					Rule* num2Rule = parseRule(num2json);
					if (num1Rule!=NULL&&num1Rule->getBaseType() == RULE_BASE_TYPE_NUMBER
						&&num2Rule!=NULL&&num2Rule->getBaseType() == RULE_BASE_TYPE_NUMBER) {
						if (key == "$lt") {
							compareRule = new LtRule((NumberRule*)num1Rule, (NumberRule*)num2Rule);
						}
						if (key == "$le") {
							compareRule = new LeRule((NumberRule*)num1Rule, (NumberRule*)num2Rule);
						}
						if (key == "$gt") {
							compareRule = new GtRule((NumberRule*)num1Rule, (NumberRule*)num2Rule);
						}
						if (key == "$ge") {
							compareRule = new GeRule((NumberRule*)num1Rule, (NumberRule*)num2Rule);
						}
					}
					if (compareRule != NULL) {
						rules.push_back(compareRule);
					}
				}
			}
		}
	}
	return rules;
}

Rule* FeatureLibrary::parseRule(nlohmann::json rulejson) {
	Rule* rule = NULL;

	vector<Rule*> rules = parseRules(rulejson);
	if (rules.size()>0) {
		rule = rules.at(0);
	}

	return rule;
}

FeatureLibraryItem::FeatureLibraryItem() {
	features = new vector<Feature*>();
}

std::string FeatureLibrary::toJson(int* state){
	json jsonFeatureLibrary;
	jsonFeatureLibrary["version"] = FIREDOG_FEATURE_LIBRARY_VERSION;

	if (items!=NULL) {
		std::vector<json> jsonItems;
		for (int i = 0; i < items->size(); i++) {
			json jsonItem;

			FeatureLibraryItem* item = items->at(i);

			jsonItem["name"] = item->name;
			jsonItem["describe"] = item->describe;
			jsonItem["author"] = item->author;

			std::vector<json> jsonFeatures;
			if (item->features != NULL && item->features->size() > 0) {
				for (int y = 0; y < item->features->size(); y++) {
					Feature* feature = item->features->at(y);
					json jsonFeature;
					jsonFeature["key"] = feature->key;
					if (!feature->hex.empty()) {
						jsonFeature["hex"] = feature->hex;
					}
					else {
						jsonFeature["text"] = feature->text;
					}
					jsonFeatures.push_back(jsonFeature);
				}
			}
			else {
				*state = FL_CONTENT_EXIST_EMPTY_FEATURES_FAIL;
				return "";
			}
			jsonItem["features"] = jsonFeatures;

			bool ruleState = false;
			json ruleJson = ruleToJson(item->rule,&ruleState);

			if (!ruleState) {
				*state = FL_CONTENT_EXIST_RULE_FORMATE_FAIL;
				return "";
			}

			jsonItem["rule"] = ruleJson;
			jsonItems.push_back(jsonItem);
		}
		jsonFeatureLibrary["items"] = jsonItems;
	}
	else {
		*state = FL_CONTENT_ITEMS_IS_EMPTY;
		return "";
	}
	std::string json = jsonFeatureLibrary.dump(4);
	*state = NO_ERROR;
	return json;
}

nlohmann::json FeatureLibrary::ruleToJson(mountcloud::Rule* rule,bool* state) {
	json jsonRule;
	//if (rule != NULL) {
	//	if (!rule->id.empty()) {
	//		jsonRule = rule->id;
	//		*state = true;
	//	}
	//	else if(rule->ands!=NULL&&rule->ands->size()>0){
	//		vector<json> andsjson;
	//		for (int i = 0; i < rule->ands->size(); i++) {
	//			mountcloud::Rule* andRule = rule->ands->at(i);
	//			bool andRuleState = false;

	//			json andJson = ruleToJson(andRule, &andRuleState);
	//			if (!andRuleState) {
	//				*state = false;
	//				return jsonRule;
	//			}
	//			andsjson.push_back(andJson);
	//		}
	//		*state = true;
	//		jsonRule["$and"] = andsjson;
	//	}
	//	else if(rule->ors!=NULL&&rule->ors->size()>0){
	//		vector<json> orsjson;
	//		for (int i = 0; i < rule->ors->size(); i++) {
	//			mountcloud::Rule* orRule = rule->ors->at(i);
	//			bool orRuleState = false;

	//			json orJson = ruleToJson(orRule, &orRuleState);
	//			if (!orRuleState) {
	//				*state = false;
	//				return jsonRule;
	//			}
	//			orsjson.push_back(orJson);
	//		}
	//		*state = true;
	//		jsonRule["$or"] = orsjson;
	//	}
	//	else {
	//		*state = false;
	//	}
	//}
	//else {
	//	*state = false;
	//}
	return jsonRule;
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