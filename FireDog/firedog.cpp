#include "firedog.h"
#include "featurelibrary.h"

using namespace firedog;

int FireDog::pushFeatureLibrary(FeatureLibrary flib) {
	this->librarys.push_back(flib);
	return NO_ERROR;
}

