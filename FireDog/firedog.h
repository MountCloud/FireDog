#ifndef _FIREDOG_FIREDOG_H_
#define _FIREDOG_FIREDOG_H_

#include <vector>
#include <stdlib.h>
#include "errorcode.h"

/// <summary>
/// fire dog main header
/// </summary>

namespace firedog {

	class FeatureLibrary;

	class MatcherDataSource;

	class Matcher;

	class FireDog {
	public:
		/// <summary>
		/// load feature library
		/// </summary>
		/// <param name="flib">Feature Library</param>
		/// <returns>errorcode</returns>
		int pushFeatureLibrary(FeatureLibrary* flib);

		/// <summary>
		/// create new matcher
		/// </summary>
		/// <returns>matcher</returns>
		Matcher* createNewMatcher();

		~FireDog();
	private:
		
		/// <summary>
		/// matcher data source
		/// </summary>
		MatcherDataSource* matcherDataSource = NULL;
	};

}

#endif

