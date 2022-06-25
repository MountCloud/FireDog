#ifndef _FIREDOG_FEATURELIBRARY_H_
#define _FIREDOG_FEATURELIBRARY_H_

#include <string>
#include <vector>

#include "config.h"
#include "json/json-schema/json-schema.hpp"
#include "rule/rule.h"

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

namespace c4 { 
	namespace yml {
		class NodeRef;
    }
};

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
        static FeatureLibrary* createByYaml(std::string yaml, int* errorcode);

		/// <summary>
		/// library version
		/// Used to check whether it matches the current data version.
		/// </summary>
		std::string version;
		
		/// <summary>
		/// Binary Feature
		/// </summary>
		std::vector<FeatureLibraryItem*>* items = NULL;

        /// <summary>
        /// to json
        /// </summary>
        /// <returns></returns>
        std::string toJson(int* state);

		FeatureLibrary();
		~FeatureLibrary();

	private:
		static std::vector<mountcloud::Rule*> parseRulesByJson(nlohmann::json rulejson);
        static mountcloud::Rule* parseRuleByJson(nlohmann::json rulejson);

        static std::vector<mountcloud::Rule*> parseRulesByYaml(c4::yml::NodeRef node);
		static mountcloud::Rule* parseRuleByYaml(c4::yml::NodeRef node);
        /// <summary>
        /// rule to json object
        /// </summary>
        /// <param name="rule"></param>
        /// <returns></returns>
        nlohmann::json ruleToJson(mountcloud::Rule* rule,bool* state);
	};


}

#endif