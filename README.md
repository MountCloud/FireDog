# FireDog
开源跨平台轻量级病毒特征检测引擎。Open source cross-platform lightweight virus signature detection engine.

# 语言 Language
C++ 11

# LICENSE

是你们喜欢的：MTI License.

让我们搞起来！

It's what you like:MTI License.

Let's do it!

# 交流 contact us

QQGroup: 337571436

Email: mountcloud@outlook.com

# 简介 Introduction

提供一个轻量级的特征检测引擎，支持自定义扩展特征库，等有时间再画引擎架构图。

Provide a lightweight feature detection engine, support custom extended feature libraries, and draw engine architecture diagrams when you have time.

# 更新日志 Update log

# v1.1.0
```
1：重构特征库格式，特征库更加合理。
2：升级hex检测，支持通配符，例如：6D ?? ?5 6? [73-75] [41-5A,61-7A] 6C 6F 75 64
3：加入轻量级规则引擎/rule/rule.h，所以特征库支持使用规则进行条件匹配，语法类似mongodb的查询。

1: Refactor the format of the feature library to make the feature library more reasonable.
2: Upgrade hex detection to support wildcards, for example: 6D ??? 5 6? [73-75] [41-5A,61-7A] 6C 6F 75 64
3: Add the lightweight rule engine /rule/rule.h, so the signature database supports the use of rules for condition matching, and the syntax is similar to mongodb query.
```

# v1.0.0 

```
支持hex、md5、text类型检测。
Support hex, md5, text type detection.
```

# 使用 Use

## Step 1: download
```
git clone https://github.com/MountCloud/FireDog.git
```
## Step 2: include
```
include FireDog
```
## Step 3: Example

```
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

    //test featureLibrary 定义特征库
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

    //bytes 校验的bytes
	  //const string bytes = "fire dog hello world.";
    const string bytes = "123213123213 mountcloud 12312312313 firedog";

    int ecode = NO_ERROR;

	  //step1: init feature library 初始化特征库
    FeatureLibrary* featureLibrary = FeatureLibrary::createByJson(featureLibraryJson, &ecode);

    if (ecode != NO_ERROR) {
        cout << "feature library load faild";
        return;
    }

    //step2: push to firedog datasource,support multiple feature libraries
    //将特征库添加到引擎中，支持多个特征库
    FireDog* fireDog = new FireDog();
    ecode = fireDog->pushFeatureLibrary(featureLibrary);
    if (ecode != NO_ERROR) {
        cout << "push feature library faild";
        return;
    }

    //step3: create match，One matcher per file!
    //创建检测器，一个检测器对应一个文件！
    Matcher* matcher = fireDog->createNewMatcher();
    if (matcher == NULL) {
        cout << "create matcher faild";
        return;
    }

    //step4: matcher bytes
    int length = bytes.length();
    MatcherResult* mr = NULL;

    //test check multiple bytes 一次放入多个byte
    //检测
    //mr = matcher->matchBytes(bytes.c_str(), length);
    //or Check byte one by one 或者一个byte一个的检测
    for (int i = 0; i < bytes.size(); i++) {
        char byte = bytes[i];
        mr = matcher->matchByte(byte);
        if (mr != NULL) {
            break;
        }
    }
    //检测结果
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
```

# 特征库示例 Example of feature library
请参考 Please refer to：doc/test-feature-library.json,doc/firedog-featurelibrary-json-schema.json
```
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
            "describe":"match we can do it., if ((str1&&str2)|(str3&&str4))&&(str1&&str4)",
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
```