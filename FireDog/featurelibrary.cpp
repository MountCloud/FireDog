#include "featurelibrary.h"

using namespace nlohmann;
using nlohmann::json_schema::json_validator;

using namespace mountcloud;
using namespace firedog;
using namespace std;

#define RYML_SINGLE_HDR_DEFINE_NOW
#include "yaml/yaml.hpp"

#include "stringutil.h"

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

				vector<Rule*> rules = parseRulesByJson(rulejson);
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

FeatureLibrary* FeatureLibrary::createByYaml(std::string yaml, int* errorcode) {

	FeatureLibrary* result = NULL;

	ryml::Tree tree;

	bool parseStatus = false;
	try {
		tree = ryml::parse_in_place(ryml::to_substr(yaml));
		parseStatus = true;
	}
	catch (std::runtime_error const&) {
		parseStatus = false;

	}
	auto rootref = tree.rootref();
	if (!parseStatus || !rootref.has_child("version") || !rootref.has_child("items")) {
		*errorcode = FL_CONTENT_FORMATE_ERROR;
		return result;
	}

	//get version
	string version;
	tree["version"] >> version;

	//check version
	if (version != FIREDOG_FEATURE_LIBRARY_VERSION) {
		*errorcode = FL_CONTENT_VERSION_ERROR;
	}

	result = new FeatureLibrary();

	result->version = version;

	auto items = tree["items"];

	if (items.is_seq()) {
		int itemscount = items.num_children();
		for (int i = 0; i < itemscount; i++) {
			auto item = items[i];
			if (item.is_map()
				&& item.has_child("author")
				&& item.has_child("name")
				&& item.has_child("describe")
				&& item.has_child("features") && item["features"].is_seq()
				&& item.has_child("rules") && item["rules"].is_seq()) {

				//先看规则
				c4::yml::NodeRef rulesYml = item["rules"];

				vector<Rule*> rules = parseRulesByYaml(rulesYml);
				if (rules.size() == 0) {
					continue;
				}

				Rule* rule = NULL;
				if (rules.size() == 1) {
					rule = rules.at(0);
				}
				else {
					AndRule* andRule = new AndRule();
					andRule->addRules(rules);
				}

				FeatureLibraryItem* fitem = new FeatureLibraryItem();
				item["author"] >> fitem->author;
				item["name"] >> fitem->name;
				item["describe"] >> fitem->describe;
				fitem->rule = rule;

				auto features = item["features"];
				int featurescount = features.num_children();
				for (int y = 0; y < featurescount; y++) {
					auto feature = features[y];
					if (feature.has_child("key")
						&& (feature.has_child("hex")
							|| feature.has_child("text"))) {
						string fkey;
						feature["key"] >> fkey;
						string fhex = "";
						string ftext = "";
						if (feature.has_child("hex")) {
							feature["hex"] >> fhex;
						}

						if (feature.has_child("text")) {
							feature["text"] >> ftext;
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

vector<Rule*> FeatureLibrary::parseRulesByJson(nlohmann::json rulejson) {
	vector<Rule*> rules;

	if (rulejson.is_string()) {
		string id = rulejson;
		CountRule* countRule = new CountRule(id);
		rules.push_back(countRule);
	}else if (rulejson.is_object()&&!rulejson.is_array()) {
		for (auto ruleiter = rulejson.begin(); ruleiter != rulejson.end(); ruleiter++) {
			std::string key = ruleiter.key();
			nlohmann::json childrulejson = rulejson[key];
			if (key == "$and" || key == "$or" || key == "$not") {
				LogicRule* logicRule = NULL;
				vector<Rule*> childRules = parseRulesByJson(childrulejson);
				if (childRules.size()>0) {
					if (key == "$and") {
						logicRule = new AndRule();
					}
					if (key == "$or") {
						logicRule = new OrRule();
					}
					if (key == "$not") {
						logicRule = new NotRule();
					}
					logicRule->addRules(childRules);
					rules.push_back(logicRule);
				}
			}
			else if (key == "$int") {
				if (childrulejson.is_number()) {
					long num = childrulejson;
					IntRule* intRule = new IntRule(childrulejson);
					rules.push_back(intRule);
				}
			}
			else if (key == "$count") {
				vector<string> ids;
				long num = 1L;

				if (childrulejson.is_string()) {
					string id = childrulejson;
					ids.push_back(id);
				}else if (childrulejson.is_array()) {
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
				}

				if (ids.size() > 0) {
					CountRule* countRule = new CountRule(ids);
					countRule->setCritical(num);
					rules.push_back(countRule);
				}
			}
			else if(key == "$lt"|| key == "$le" || key == "$gt" || key == "$ge") {
				Rule* compareRule = NULL;

				vector<Rule*> childRules = parseRulesByJson(childrulejson);

				if (childRules.size()==2) {

					Rule* num1Rule = childRules[0];
					Rule* num2Rule = childRules[1];
					if (num1Rule != NULL && num1Rule->getBaseType() == RULE_BASE_TYPE_NUMBER
						&& num2Rule != NULL && num2Rule->getBaseType() == RULE_BASE_TYPE_NUMBER) {
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

Rule* FeatureLibrary::parseRuleByJson(nlohmann::json rulejson) {
	Rule* rule = NULL;

	vector<Rule*> rules = parseRulesByJson(rulejson);
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

std::vector<mountcloud::Rule*> FeatureLibrary::parseRulesByYaml(c4::yml::NodeRef node) {
	std::vector<mountcloud::Rule*> rules;
	if (node.is_seq()) {
		int count = node.num_children();
		for (int i = 0; i < count; i++) {
			std::vector<mountcloud::Rule*> crules = parseRulesByYaml(node[i]);
			if (crules.size() > 0) {
				rules.insert(rules.end(), crules.begin(), crules.end());
			}
		}
	}
	else if(node.is_map()){
		c4::yml::Tree* tree = node.tree();
		int nodeid = node.id();
		for (size_t child_id = tree->first_child(nodeid); child_id != ryml::NONE; child_id = tree->next_sibling(child_id)) {
			auto key = tree->key(child_id);
			auto childrule = node[key];
			if (key == "$and" || key == "$or" || key == "$not") {
				LogicRule* logicRule = NULL;
				vector<Rule*> childRules = parseRulesByYaml(childrule);
				if (childRules.size() > 0) {
					if (key == "$and") {
						logicRule = new AndRule();
					}
					if (key == "$or") {
						logicRule = new OrRule();
					}
					if (key == "$not") {
						logicRule = new NotRule();
					}
					logicRule->addRules(childRules);
					rules.push_back(logicRule);
				}
			}
			else if (key == "$int") {
				if (childrule.is_keyval()) {
					long num = 0L;
					string numstr;
					childrule >> numstr;
					if (StringUtil::isNumber(numstr)) {
						num = atoi(numstr.c_str());
						IntRule* intRule = new IntRule(num);
						rules.push_back(intRule);
					}
				}
			}
			else if (key == "$count") {
				vector<string> ids;
				long num = 1L;

				if (childrule.is_keyval()) {
					string id;
					childrule >> id;
					ids.push_back(id);
				}
				else if (childrule.is_seq()) {
					int idslen = childrule.num_children();
					for (int i = 0; i < idslen; i++) {
						auto crj = childrule[i];
						string id;
						crj >> id;
						ids.push_back(id);
					}
				}

				if (ids.size() > 0) {
					CountRule* countRule = new CountRule(ids);
					countRule->setCritical(num);
					rules.push_back(countRule);
				}
			}
			else if (key == "$lt" || key == "$le" || key == "$gt" || key == "$ge") {
				Rule* compareRule = NULL;

				vector<Rule*> childRules = parseRulesByYaml(childrule);

				if (childRules.size() == 2) {

					Rule* num1Rule = childRules[0];
					Rule* num2Rule = childRules[1];
					if (num1Rule != NULL && num1Rule->getBaseType() == RULE_BASE_TYPE_NUMBER
						&& num2Rule != NULL && num2Rule->getBaseType() == RULE_BASE_TYPE_NUMBER) {
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

mountcloud::Rule* FeatureLibrary::parseRuleByYaml(c4::yml::NodeRef node) {
	vector<Rule*> rules = parseRulesByYaml(node);
	if (rules.size() > 0) {
		return rules[0];
	}
	return NULL;
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