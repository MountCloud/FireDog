#include "rule.h"
#include <stdexcept>

using namespace mountcloud;

RuleData::RuleData() {
	this->data = new std::unordered_map<std::string, long>();
}

RuleData::~RuleData() {
	if (data != NULL) {
		data->clear();
		delete data;
		data = NULL;
	}
}

void RuleData::hit(std::string id) {
	long count = data->operator[](id);
	data->operator[](id) = (count + 1);
}

bool RuleData::has(std::string id) {
	bool state = false;
	try {
		state = data->operator[](id) > 0;
	}
	catch (std::out_of_range ex) {
		state = false;
	}
	return state;
}

long RuleData::count(std::string id) {
	long count = data->operator[](id);
	return count;
}

Rule::Rule() {
	rules = new std::vector<Rule*>();
}

Rule::~Rule() {
	if (rules != NULL) {
		for (int i = 0; i < rules->size(); i++) {
			Rule* rule = rules->at(i);
			delete rule;
			rule = NULL;
		}
		delete rules;
		rules = NULL;
	}
}

void Rule::addRule(Rule* rule) {
	rules->push_back(rule);
}

bool AndRule::check(RuleData* data) {
	//and的情况
	if (rules != NULL && rules->size() > 0) {
		for (int i = 0; i < rules->size(); i++) {
			Rule* r = rules->at(i);
			if (!r->check(data)) {
				return false;
			}
		}
		return true;
	}
}

bool OrRule::check(RuleData* data) {
	//or
	if (rules != NULL && rules->size() > 0) {
		for (int i = 0; i < rules->size(); i++) {
			Rule* r = rules->at(i);
			if (r->check(data)) {
				return true;
			}
		}
	}
}

//Rule::~Rule() {
//	if (ands != NULL) {
//		for (int i = 0; i < ands->size(); i++) {
//			Rule* r = ands->at(i);
//			if (r != NULL) {
//				delete r;
//				r = NULL;
//			}
//		}
//		ands->clear();
//		delete ands;
//		ands = NULL;
//	}
//
//	if (ors != NULL) {
//		for (int i = 0; i < ors->size(); i++) {
//			Rule* r = ors->at(i);
//			if (r != NULL) {
//				delete r;
//				r = NULL;
//			}
//		}
//		ors->clear();
//		delete ors;
//		ors = NULL;
//	}
//}
//
//bool Rule::check(RuleData* data) {
//	//and的情况
//	if (ands != NULL && ands->size() > 0) {
//		for (int i = 0; i < ands->size(); i++) {
//			Rule* r = ands->at(i);
//			if (!r->check(data)) {
//				return false;
//			}
//		}
//		return true;
//	}
//
//	//or
//	if (ors != NULL && ors->size() > 0) {
//		for (int i = 0; i < ors->size(); i++) {
//			Rule* r = ors->at(i);
//			if (r->check(data)) {
//				return true;
//			}
//		}
//	}
//
//	//ids
//	if (!id.empty()) {
//		if (data->has(id)) {
//			return true;
//		}
//	}
//
//	return false;
//}