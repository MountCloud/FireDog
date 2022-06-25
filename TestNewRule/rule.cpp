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

AllRule::AllRule(std::string id) {
	this->ids.push_back(id);
}

AllRule::AllRule(std::vector<std::string> ids) {
	this->ids.insert(this->ids.end(), ids.begin(), ids.end());
}

void AllRule::addId(std::string id) {
	this->ids.push_back(id);
}

bool AllRule::check(RuleData* data) {
	for (int i = 0; i < ids.size(); i++) {
		if (!data->has(ids.at(i))) {
			return false;
		}
	}
	return true;
}

LogicRule::LogicRule() {
	rules = new std::vector<Rule*>();
}

LogicRule::~LogicRule() {
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

void LogicRule::addRule(Rule* rule) {
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

bool NotRule::check(RuleData* data) {
	//not
	if (rules != NULL && rules->size() > 0) {
		for (int i = 0; i < rules->size(); i++) {
			Rule* r = rules->at(i);
			if (r->check(data)) {
				return false;
			}
		}
		return true;
	}
}

IntRule::IntRule(long num) {
	this->num = num;
}

bool IntRule::check(RuleData* data) {
	return true;
}

long IntRule::getNumber(RuleData* data) {
	return this->num;
}

CountRule::CountRule(std::string id) {
	this->ids.push_back(id);
}

CountRule::CountRule(std::vector<std::string> ids) {
	if (ids.size() > 0) {
		for (int i = 0; i < ids.size(); i++) {
			this->addId(ids.at(i));
		}
	}
}

void CountRule::addId(std::string id) {
	if (std::find(this->ids.begin(),this->ids.end(),id) == this->ids.end()) {
		this->ids.push_back(id);
	}
}

void CountRule::addIds(std::vector<std::string> ids){
	if (ids.size()>0) {
		for (int i = 0; i < ids.size(); i++) {
			this->addId(ids.at(i));
		}
	}
}

long CountRule::getNumber(RuleData* data) {
	long sumCount = 0;
	for (int i = 0; i < this->ids.size(); i++) {
		long count = data->count(this->ids.at(i));
		sumCount += count;
	}
	return sumCount;
}

void CountRule::setCritical(long critical) {
	this->critical = critical;
}

bool CountRule::check(RuleData* data) {
	long sumCount = this->getNumber(data);
	if (sumCount > critical) {
		return true;
	}
	return false;
}


CompareRule::CompareRule(NumberRule* num1, NumberRule* num2) {
	this->num1 = num1;
	this->num2 = num2;
}

LtRule::LtRule(NumberRule* num1, NumberRule* num2) : CompareRule(num1,num2) {
}

bool LtRule::check(RuleData* data) {
	long n1 = num1->getNumber(data);
	long n2 = num2->getNumber(data);
	if (n1 < n2) {
		return true;
	}
	return false;
}

LeRule::LeRule(NumberRule* num1, NumberRule* num2) : CompareRule(num1, num2) {
}

bool LeRule::check(RuleData* data) {
	long n1 = num1->getNumber(data);
	long n2 = num2->getNumber(data);
	if (n1 <= n2) {
		return true;
	}
	return false;
}


GtRule::GtRule(NumberRule* num1, NumberRule* num2) : CompareRule(num1, num2) {
}

bool GtRule::check(RuleData* data) {
	long n1 = num1->getNumber(data);
	long n2 = num2->getNumber(data);
	if (n1 > n2) {
		return true;
	}
	return false;
}


GeRule::GeRule(NumberRule* num1, NumberRule* num2) : CompareRule(num1, num2) {
}

bool GeRule::check(RuleData* data) {
	long n1 = num1->getNumber(data);
	long n2 = num2->getNumber(data);
	if (n1 >= n2) {
		return true;
	}
	return false;
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