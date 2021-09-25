#ifndef _FIREDOG_FEATURELIBRARY_H_
#define _FIREDOG_FEATURELIBRARY_H_

#include <string>
#include <vector>

#include "json/json.hpp"
#include "errorcode.h"

#define FEATURE_TYPE_HEX	1
#define FEATURE_TYPE_MD5	2
#define FEATURE_TYPE_TEXT	3


static const char* FIREDOG_FEATURE_LIBRARAY_JSON_SCHEMA = R"(

)";

static const char* FIREDOG_FEATURE_LIBRARAY_HEX_REG = "";


/// <summary>
/// Feature Library h
/// </summary>
namespace firedog {

	class FeatureLibraryItem {
	public:

		/// <summary>
		/// library author
		/// </summary>
		std::string author;

		/// <summary>
		/// create time utc str
		/// </summary>
		std::string createTime;

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
		/// create by json
		/// </summary>
		static FeatureLibrary createByJson(std::string json, int* errorcode);

		/// <summary>
		/// library version
		/// Used to check whether it matches the current data version.
		/// </summary>
		std::string version;
		
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
		/// load by json
		/// </summary>
		/// <param name="json">json string</param>
		/// <returns>errorcode</returns>
		int loadByJson(std::string json);

		/// <summary>
		/// parse json to library item
		/// </summary>
		/// <param name="jsonValue">json value</param>
		/// <returns>item</returns>
		void parseJson(std::vector<FeatureLibraryItem> *items, nlohmann::json values);
	};


}

#endif