#ifndef _FIREDOG_FIREDOG_H_
#define _FIREDOG_FIREDOG_H_

#include <vector>
#include "errorcode.h"

/// <summary>
/// fire dog main header
/// </summary>

namespace firedog {

	class FeatureLibrary;

	class FireDog {
	public:
		/// <summary>
		/// load feature library
		/// </summary>
		/// <param name="flib">Feature Library</param>
		/// <returns>errorcode</returns>
		int pushFeatureLibrary(FeatureLibrary flib);
	private:

		/// <summary>
		/// libs
		/// </summary>
		std::vector<FeatureLibrary> librarys;
		
		
	};

}

#endif

