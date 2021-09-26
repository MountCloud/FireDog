// TestWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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
            "version":"1.1.0",
            "items":[
                {
                    "name":"test simple",
                    "describe":"match hello world,hello hex:68 65 6C 6C 6F,world hex:77 6F 72 6C 64",
                    "author":"MountCloud",
                    "features":[
                        {
                            "key":"str1",
                            "text":"hello"
                        },
                        {
                            "key":"str2",
                            "text":"world"
                        }
                    ],
                    "rule":{
                        "$and":[
                            "str1",
                            "str2"
                        ]
                    }
                },
                {
                    "name":"test complex hex match",
                    "describe":"match mountcloud firedog,mountcloud hex:6D 6F 75 6E 74 63 6C 6F 75 64,firedog hex:66 69 72 65 64 6F 67",
                    "author":"MountCloud",
                    "features":[
                        {
                            "key":"hex1",
                            "hex":"6D ?? ?5 6? [73-75] [41-5A,61-7A] 6C 6F 75 64"
                        },
                        {
                            "key":"hex2",
                            "hex":"66 6? 72 ?? 64 [60-70] 67"
                        }
                    ],
                    "rule":{
                        "$and":[
                            "hex1",
                            "hex2"
                        ]
                    }
                },
                {
                    "name":"test complex rule match",
                    "describe":"match we can do it.",
                    "author":"MountCloud",
                    "features":[
                        {
                            "key":"str1",
                            "text":"we"
                        },
                        {
                            "key":"str2",
                            "text":"can"
                        },
                        {
                            "key":"str3",
                            "text":"do"
                        },
                        {
                            "key":"str4",
                            "text":"it"
                        }
                    ],
                    "rule":{
                        "$and":[
                            {
                                "$or":[
                                    {
                                        "$and":[
                                            "str1","str2"
                                        ]
                                    },
                                    {
                                        "$and":[
                                            "str3","str4"
                                        ]
                                    }
                                ]
                            },
                            {
                                "$and":[
                                    "str1",
                                    "str4"
                                ]
                            }
                        ]
                    }
                }
            ]
        }

	)";

    //bytes
	//const string bytes = "fire dog hello world.";
    const string bytes = "123213123213 mountcloud 12312312313 firedog";

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
    MatcherResult* mr = NULL;

    //test match bytes
    //mr = matcher->matchBytes(bytes.c_str(), length);
    //or
    for (int i = 0; i < bytes.size(); i++) {
        char byte = bytes[i];
        mr = matcher->matchByte(byte);
        if (mr != NULL) {
            break;
        }
    }

    if (mr != NULL) {
        cout << "found it:" << mr->name << endl;
    }
    else {
        cout << "not found." << endl;
    }
}

int main()
{
    testMatcher();
    return 0;
}