#ifndef _FIREDOG_FEATURELIBRARY_H_
#define _FIREDOG_FEATURELIBRARY_H_

#include <string>
#include <vector>

#include "json/json.hpp"
#include "errorcode.h"

#define FEATURE_TYPE_HEX	1
#define FEATURE_TYPE_TEXT	2

#define FEATURE_TYPE_HEX_STR	"hex"
#define FEATURE_TYPE_TEXT_STR	"text"


static const char* FIREDOG_FEATURE_LIBRARAY_JSON_SCHEMA = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "FireDog FeatureLibrary",
        "properties": {
            "version": {
                "description": "feature library version",
                "type": "string"
            },
            "items": {
                "description": "feature library items",
                "type": "array",
                "items": {
                    "type": "object",
                    "properties": {
                        "author":{
                            "type":"string"
                        },
                        "name":{
                            "type":"string"
                        },
                        "describe":{
                            "type":"string"
                        },
                        "features":{
                            "type":"array",
                            "items":{
                                "type": "object",
                                "properties": {
                                    "key":{
                                        "type":"string"
                                    },
                                    "hex":{
                                        "type":"string",
                                        "pattern":"^(([a-fA-F0-9?]{2}[ ]?)|(\\[([0-9a-fA-F]{2}[ ]?-[ ]?[a-fA-F0-9]{2}[ ]?[,]?){1,}\\][ ]?)){0,}$"
                                    }
                                    ,
                                    "text":{
                                        "type":"string"
                                    }
                                },
                                "required": [
                                    "key"
                                ]
                            },
                            "required": [
                                "author",
                                "name",
                                "describe",
                                "features",
                                "rule"
                            ]
                        },
                        "rule":{
                            "type":"object"
                        }
                    },
                    "required": [
                        "author",
                        "name",
                        "describe",
                        "features",
                        "rule"
                    ]
                }
            }
        },
        "required": [
            "version",
            "items"
        ],
        "type": "object"
    }
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
		std::string key;
		std::string hex;
        std::string text;
	};

	class FeatureLibraryItem {
	public:
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

		/// <summary>
		/// check rule
		/// </summary>
		mountcloud::Rule* rule = NULL;

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