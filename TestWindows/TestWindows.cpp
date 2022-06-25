// TestWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma warning(disable: 4996) 

#include <iostream>
#include <string>
#include <iomanip>

#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"

#include "stringutil.h"

#include "rule/rule.h"


using namespace std;
using namespace firedog;

void testMatcher() {

    //test featureLibrary
	const char* featureLibraryJson = R"(

{
    "items": [
        {
            "author": "mountcloud",
            "describe": "t",
            "features": [
                {
                    "key": "t",
                    "text": "t"
                },{
                    "key": "a",
                    "text": "a"
                }
            ],
            "name": "t",
            "rule": {
                "$and": {
					"$gt":{
						"$count":"t",
						"$int" : 5
					},
					"$not":{
						"$gt":{
							"$count":"t",
							"$int" : 10
						}
					},
					"$gt":{
						"$count":"a",
						"$int" : 3
					},
					"$lt":{
						"$count":"a",
						"$int" : 5
					}
                }
            }
        }
    ],
    "version": "1.2.0"
}

	)";

    //bytes
	//const string bytes = "fire dog hello world.";
    const string bytes = "ttttttaaa";

    int ecode = NO_ERROR;

	//step1: init feature library
    FeatureLibrary* featureLibrary = FeatureLibrary::createByJson(featureLibraryJson, &ecode);

    if (ecode != NO_ERROR) {
        cout << "feature library load faild";
        return;
    }

    //step2: push to firedog datasource
    FireDog* fireDog = new FireDog();
    ecode = fireDog->pushFeatureLibrary(featureLibrary);
    if (ecode != NO_ERROR) {
        cout << "push feature library faild";
        return;
    }

    //step3: create match，One matcher per file!
    Matcher* matcher = fireDog->createNewMatcher();
    if (matcher == NULL) {
        cout << "create matcher faild";
        return;
    }

    //step4: matcher bytes
    int length = bytes.length();

    vector<MatcherResult*>* mrs = NULL;
    //test match bytes
    //mr = matcher->matchBytes(bytes.c_str(), length);
    //or
    for (int i = 0; i < bytes.size(); i++) {
        char byte = bytes[i];
        mrs = matcher->matchByte(byte);
        if (mrs != NULL) {
            break;
        }
    }

    if (mrs != NULL) {
        cout << "found it:" << mrs->size() << endl;
    }
    else {
        cout << "not found." << endl;
    }
}

struct YamlErrorHandler
{
    bool iserror = false;
	// this will be called on error
	void on_error(const char* msg, size_t len, ryml::Location loc)
	{
        iserror = true;
	}

	// bridge
	ryml::Callbacks callbacks()
	{
		return ryml::Callbacks(this, nullptr, nullptr, YamlErrorHandler::s_error);
	}
	static void s_error(const char* msg, size_t len, ryml::Location loc, void* this_)
	{
		return ((YamlErrorHandler*)this_)->on_error(msg, len, loc);
	}

	// save the default callbacks for checking
	YamlErrorHandler() : defaults(ryml::get_callbacks()) {}
	ryml::Callbacks defaults;
} YamlErrorHandler;

//void testyaml() {
//	const char* featureLibraryJson = R"(
//
//items:
//  - author: mountcloud
//    describe: t
//    features:
//      - key: t
//        text: t
//      - key: a
//        text: a
//    name: t
//    rules:
//      - $and:
//          - $gt:
//              $count: a
//              $int: 3
//          - $not:
//              - $gt:
//                  $count: t
//                  $int: 10
//          - $lt:
//              $count: a
//              $int: 5
//version: 1.2.0
//
//	)";
//
//	// set a global error handler. Note the error callback must never
//	// return: it must either abort or throw an exception. Otherwise,
//	// the parser will enter into an infinite loop, or the program may
//	// crash.
//    ryml::set_callbacks(ryml::Callbacks(NULL, NULL, NULL, [](const char* msg, size_t len, ryml::Location loc, void* this_) {
//        
//        }));
//
//    std::string contents = featureLibraryJson;
//
//    bool iserror = false;
//
//	char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
//    ryml::Tree tree;
//
//	try { 
//        tree = ryml::parse_in_place(ryml::to_substr(contents));
//    }
//	catch (std::runtime_error const&) { 
//        iserror = true;
//    }
//
//
//
//    size_t root_id = tree.root_id();
//    string version;
//    auto versionnode = tree.rootref().has_child("version");
//    auto status = tree.rootref().has_child("items");
//    status = tree["items"].is_seq();
//    status = tree["version"].is_seq();
//
//	size_t count = 0;
//    for (size_t child_id = tree.first_child(root_id); child_id != ryml::NONE; child_id = tree.next_sibling(child_id)) {
//        auto key = tree.key(child_id);
//        if (key == "version") {
//            tree[key] >> version;
//            cout << version << endl;
//        }
//    }
//    auto rules = tree["items"][0]["rules"];
//
//    for (int i = 0; i < rules.num_children(); i++) {
//        auto rule = rules[i];
//		for (size_t child_id = tree.first_child(rule.id()); child_id != ryml::NONE; child_id = tree.next_sibling(child_id)) {
//			auto key = tree.key(child_id);
//
//			string keyst;
//			if (key == "$and") {
//				cout << "is and " << endl;
//			}
//			if (key == "or") {
//				cout << "is and " << endl;
//			}
//
//		}
//    }
//    
//
//
//    int num = tree["items"].num_children();
//    for (int i = 0; i < num; i++) {
//        auto item = tree["items"][i];
//        
//    }
//
//    int ii = 0;
//}

int main()
{
    //testMatcher();

    //testyaml();

    return 0;
}