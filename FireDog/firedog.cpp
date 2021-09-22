#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"

using namespace firedog;

int FireDog::pushFeatureLibrary(FeatureLibrary flib) {

	if (this->matcherDataSource == NULL) {
		this->matcherDataSource = new MatcherDataSource();
	}

	int pushresult = this->matcherDataSource->pushFeatureLibrary(flib);
	if (pushresult != NO_ERROR) {
		return pushresult;
	}

	this->librarys.push_back(flib);

	return NO_ERROR;
}

Matcher* FireDog::createNewMatcher() {

	//EMPTY Library
	if (this->librarys.size()==0) {
		return NULL;
	}

	Matcher* matcher = new Matcher(this->matcherDataSource);
	return matcher;
}

FireDog::~FireDog() {
	if (librarys.size() > 0) {
		librarys.clear();
	}
	if (matcherDataSource != NULL) {
		delete matcherDataSource;
		matcherDataSource = NULL;
	}
}