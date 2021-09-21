#ifndef _FIREDOG_FIREDOG_H_
#define _FIREDOG_FIREDOG_H_

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
		int loadFeatureLibrary(FeatureLibrary flib);

	};

}

#endif

