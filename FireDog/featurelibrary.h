#ifndef _FIREDOG_FEATURELIBRARY_H_
#define _FIREDOG_FEATURELIBRARY_H_

#include <string>
#include <vector>


#include "errorcode.h"

/// <summary>
/// Feature Library h
/// </summary>
namespace firedog {

	class FeatureLibraryItem {
	public:
		/// <summary>
		/// content
		/// </summary>
		std::string content;
		/// <summary>
		/// name or title
		/// </summary>
		std::string name;
		/// <summary>
		/// describe
		/// </summary>
		std::string describe;
	};
	
	class FeatureLibrary {
	public:
		/// <summary>
		/// library version
		/// Used to check whether it matches the current data version.
		/// </summary>
		int version;
		/// <summary>
		/// load by json
		/// </summary>
		/// <param name="json">json string</param>
		/// <returns>errorcode</returns>
		int loadByJson(std::string json);
		

		/// <summary>
		/// Binary Feature
		/// </summary>
		std::vector<FeatureLibraryItem> hexItems;

		/// <summary>
		/// File MD5 Feature 
		/// </summary>
		std::vector<FeatureLibraryItem> md5Items;

		/// <summary>
		/// text Feature
		/// </summary>
		std::vector<FeatureLibraryItem> textItems;
		
	};

}

#endif