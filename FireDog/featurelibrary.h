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

namespace mountcloud {
	class Rule;
}

/// <summary>
/// Feature Library h
/// </summary>
namespace firedog {


	class Feature {
	public:
		int id;
		std::string name;
		std::string type;
		std::string content;
	};

	class FeatureLibraryItem {
	public:
		int id;
		/// <summary>
		/// library author
		/// </summary>
		std::string author;

		/// <summary>
		/// name or title
		/// </summary>
		std::string name;

		/// <summary>
		/// describe
		/// </summary>
		std::string describe;

		/// <summary>
		/// Feature
		/// </summary>
		std::vector<Feature*>* features = NULL;

		mountcloud::Rule* rule;

		FeatureLibraryItem();
		~FeatureLibraryItem();
	};
	
	class FeatureLibrary {
	public:
		/// <summary>
		/// create by json
		/// </summary>
		static FeatureLibrary* createByJson(std::string json, int* errorcode);

		/// <summary>
		/// library version
		/// Used to check whether it matches the current data version.
		/// </summary>
		std::string version;
		
		/// <summary>
		/// Binary Feature
		/// </summary>
		std::vector<FeatureLibraryItem*>* items = NULL;

		FeatureLibrary();
		~FeatureLibrary();

	private:
		static mountcloud::Rule* parseRule(nlohmann::json rulejson);
	};


}

#endif