#include "rule.h"
#include <stdexcept>

using namespace mountcloud;

RuleData::RuleData() {
	this->data = new std::unordered_map<std::string, bool>();
}

RuleData::~RuleData() {
	if (data != NULL) {
		data->clear();
		delete data;
		data = NULL;
	}
}

void RuleData::set(std::string id, bool state) {
	data->operator[](id) = state;
}

bool RuleData::get(std::string id) {
	bool state = false;
	try {
		state = data->at(id);
	}
	catch (std::out_of_range ex) {
		state = false;
	}
	return state;
}

Rule::~Rule() {
	if (ands != NULL) {
		for (int i = 0; i < ands->size(); i++) {
			Rule* r = ands->at(i);
			if (r != NULL) {
				delete r;
				r = NULL;
			}
		}
		ands->clear();
		delete ands;
		ands = NULL;
	}

	if (ors != NULL) {
		for (int i = 0; i < ors->size(); i++) {
			Rule* r = ors->at(i);
			if (r != NULL) {
				delete r;
				r = NULL;
			}
		}
		ors->clear();
		delete ors;
		ors = NULL;
	}
}

bool Rule::check(RuleData* data) {
	//andµÄÇé¿ö
	if (ands != NULL && ands->size() > 0) {
		for (int i = 0; i < ands->size(); i++) {
			Rule* r = ands->at(i);
			if (!r->check(data)) {
				return false;
			}
		}
		return true;
	}

	//or
	if (ors != NULL && ors->size() > 0) {
		for (int i = 0; i < ors->size(); i++) {
			Rule* r = ors->at(i);
			if (r->check(data)) {
				return true;
			}
		}
	}

	//ids
	if (!id.empty()) {
		if (data->get(id)) {
			return true;
		}
	}

	return false;
}