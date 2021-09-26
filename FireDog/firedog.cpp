#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"

using namespace firedog;

int FireDog::pushFeatureLibrary(FeatureLibrary* flib) {

	if (this->matcherDataSource == NULL) {
		this->matcherDataSource = new MatcherDataSource();
	}

	int pushresult = this->matcherDataSource->pushFeatureLibrary(flib);
	if (pushresult != NO_ERROR) {
		return pushresult;
	}

	return NO_ERROR;
}

Matcher* FireDog::createNewMatcher() {
	Matcher* matcher = new Matcher(this->matcherDataSource);
	return matcher;
}

FireDog::~FireDog() {
	if (matcherDataSource != NULL) {
		delete matcherDataSource;
		matcherDataSource = NULL;
	}
}