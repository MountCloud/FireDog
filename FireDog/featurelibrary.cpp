#pragma warning (disable: 4996)

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

FeatureLibrary* FeatureLibrary::createByYaml(std::string yaml, int* errorcode) {

	FeatureLibrary* result = NULL;

	ryml::Tree tree;

	bool parseStatus = false;
	try {
		tree = ryml::parse_in_place(ryml::to_substr(yaml));
		parseStatus = true;
	}
	catch (...) {
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

FeatureLibraryItem::FeatureLibraryItem() {
	features = new vector<Feature*>();
}

std::string FeatureLibrary::toYaml(int* state) {
	string ymlstr;
	ryml::Tree tree;
	try {
		c4::yml::NodeRef root = tree.rootref();
		root |= ryml::MAP;
		root["version"] << string(FIREDOG_FEATURE_LIBRARY_VERSION);
		if (items != NULL) {
			root["items"] |= ryml::SEQ;
			for (int i = 0; i < items->size(); i++) {
				FeatureLibraryItem* item = items->at(i);
				root["items"][i] |= ryml::MAP;
				root["items"][i]["name"] << item->name;
				root["items"][i]["author"] << item->author;
				root["items"][i]["describe"] << item->describe;

				if (item->features != NULL && item->features->size() > 0) {
					root["items"][i]["features"] |= ryml::SEQ;;
					for (int y = 0; y < item->features->size(); y++) {
						Feature* feature = item->features->at(y);
						root["items"][i]["features"][y] |= ryml::MAP;
						root["items"][i]["features"][y]["key"] << feature->key;
						if (!feature->hex.empty()) {
							root["items"][i]["features"][y]["hex"] << feature->hex;
						}
						else {
							root["items"][i]["features"][y]["text"] << feature->text;
						}
					}
				}
				root["items"][i]["rules"] |= ryml::SEQ;
				root["items"][i]["rules"][0] |= ryml::MAP;
				c4::yml::NodeRef ymlrules = root["items"][i]["rules"][0];
				ruleToYaml(ymlrules, item->rule);
			}
		}
		*state = 0;
	}
	catch (...) {
		*state = -1;
	}

	if (*state == 0) {
		ymlstr = ryml::emitrs<std::string>(tree);
	}

	return ymlstr;
}

void FeatureLibrary::ruleToYaml(c4::yml::NodeRef& parentNode, mountcloud::Rule* rule) {
	if (rule->getType() == RULE_BASE_TYPE_ALL) {
		AllRule* allRule = (AllRule*)rule;
		if (allRule->getIds().size() > 0) {
			parentNode["$all"] |= ryml::SEQ;
			for (int i = 0; i < allRule->getIds().size(); i++) {
				parentNode["$all"][i] << allRule->getIds().at(i);
			}
		}
	}
	//------------------------------------------------
	else if (rule->getBaseType() == RULE_BASE_TYPE_LOGIC) {
		c4::yml::NodeRef logicNode;
		if (rule->getType() == RULE_TYPE_LOGIC_AND) {
			parentNode["$and"] |= ryml::SEQ;
			logicNode = parentNode["$and"];
		}
		else if (rule->getType() == RULE_TYPE_LOGIC_OR) {
			parentNode["$or"] |= ryml::SEQ;
			logicNode = parentNode["$or"];
		}
		else if (rule->getType() == RULE_TYPE_LOGIC_NOT) {
			parentNode["$not"] |= ryml::SEQ;
			logicNode = parentNode["$not"];
		}

		LogicRule* logicRule = (LogicRule*)rule;

		for (int i = 0; i < logicRule->getRules()->size(); i++) {
			Rule* tlr = logicRule->getRules()->at(i);
			logicNode[i] |= ryml::MAP;
			c4::yml::NodeRef logicChildNode = logicNode[i];
			ruleToYaml(logicChildNode, tlr);
		}
	}
	//------------------------------------------------
	else if (rule->getType() == RULE_TYPE_NUMBER_INT) {
		IntRule* intRule = (IntRule*)rule;
		parentNode["$int"] << intRule->getNum();
	}
	else if (rule->getType() == RULE_TYPE_NUMBER_COUNT) {
		CountRule* countRule = (CountRule*)rule;
		parentNode["$count"] |= ryml::SEQ;
		for (int i = 0; i < countRule->getIds().size(); i++) {
			parentNode["$count"][i] << countRule->getIds().at(i);
		}
	}
	//------------------------------------------------
	else if (rule->getBaseType() == RULE_BASE_TYPE_COMPARE) {
		c4::yml::NodeRef compaerNode;
		if (rule->getType() == RULE_TYPE_COMPARE_LT) {
			parentNode["$lt"] |= ryml::SEQ;
			compaerNode = parentNode["$lt"];
		}
		else if (rule->getType() == RULE_TYPE_COMPARE_LE) {
			parentNode["$lt"] |= ryml::SEQ;
			compaerNode = parentNode["$lt"];
		}
		else if (rule->getType() == RULE_TYPE_COMPARE_GT) {
			parentNode["$gt"] |= ryml::SEQ;
			compaerNode = parentNode["$gt"];
		}
		else if (rule->getType() == RULE_TYPE_COMPARE_GE) {
			parentNode["$ge"] |= ryml::SEQ;
			compaerNode = parentNode["$ge"];
		}
		CompareRule* compareRule = (CompareRule*)rule;


		compaerNode[0] |= ryml::MAP;
		compaerNode[1] |= ryml::MAP;

		c4::yml::NodeRef num1Node = compaerNode[0];
		c4::yml::NodeRef num2Node = compaerNode[1];

		ruleToYaml(num1Node, compareRule->getNum1());
		ruleToYaml(num2Node, compareRule->getNum2());

	}
	
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
			if (key == "$all") {
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
					AllRule* allRule = new AllRule(ids);
					rules.push_back(allRule);
				}
			}else if (key == "$and" || key == "$or" || key == "$not") {
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
						if (StringUtil::isNumber(id)&&i==idslen-1) {
							num = atoi(id.c_str());
						}
						else {
							ids.push_back(id);
						}
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