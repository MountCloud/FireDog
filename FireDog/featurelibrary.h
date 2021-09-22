#ifndef _FIREDOG_FEATURELIBRARY_H_
#define _FIREDOG_FEATURELIBRARY_H_

#include <string>
#include <vector>

#include "errorcode.h"

#define FEATURE_TYPE_HEX	1
#define FEATURE_TYPE_MD5	2
#define FEATURE_TYPE_TEXT	3

namespace Json {
	class Value;
}

/// <summary>
/// Feature Library h
/// </summary>
namespace firedog {

	class FeatureLibraryItem {
	public:
		/// <summary>
		/// name or title
		/// </summary>
		std::string name;

		/// <summary>
		/// describe
		/// </summary>
		std::string describe;

		/// <summary>
		/// feature content,hex or md5 or text
		/// </summary>
		std::string content;
	};
	
	class FeatureLibrary {
	public:
		/// <summary>
		/// library version
		/// Used to check whether it matches the current data version.
		/// </summary>
		int version;

		/// <summary>
		/// library author
		/// </summary>
		std::string author;

		/// <summary>
		/// create time utc str
		/// </summary>
		std::string createTime;

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

	private:
		/// <summary>
		/// parse json to library item
		/// </summary>
		/// <param name="jsonValue">json value</param>
		/// <returns>item</returns>
		void parseJson(std::vector<FeatureLibraryItem> *items, Json::Value values);
	};


}

#endif