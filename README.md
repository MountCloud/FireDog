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

# Code大哥？Code big brother?

1：

特征库加载性能调优：

可以从fireDog->pushFeatureLibrary下手，也可以优化MatcherDataSource数据结构。

Performance tuning of feature library loading:

You can start from fireDog->pushFeatureLibrary, or you can optimize the MatcherDataSource data structure.

2：

特征匹配性能调优：

Matcher->matchBytes或者Matcher->matchByte，目前Matcher是单线程的，按照引擎范围的定义，多线程是交给实际的业务多线程调用引擎进行匹配，你也可以升级match方法，让他具有多线程。你可以优化MatcherDataSource的数据结构让他检索时更快。

Performance tuning of feature matcher:

Matcher->matchBytes or Matcher->matchByte, currently Matcher is single-threaded, according to the definition of the engine range, multi-threading is handed over to the actual business multi-threaded call engine for matching, you can also upgrade the match method to make it multi-threaded . You can optimize the data structure of MatcherDataSource to make retrieval faster.

3：

总而言之，尽情发挥。

All in all, play as much as you like.

# 简介 Introduction

提供一个轻量级的特征检测引擎，支持自定义扩展特征库，等有时间再画引擎架构图。

Provide a lightweight feature detection engine, support custom extended feature libraries, and draw engine architecture diagrams when you have time.

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

#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"

#include "stringutil.h"

using namespace std;
using namespace firedog;


void testOutPut() {
	string hexstr = "hello";
	cout << StringUtil::textToHexText(hexstr) << endl;
}

void testMatcher() {

    //test featureLibrary
    const char* featureLibraryJson = R"(
		{
          "name":"Test Feature Library",
          "version":"1.0.0",
          "author":"MountCloud",
          "createTime":"2021-09-23 00:58:00",
          "hexItems":[
            {
              "name":"hello world hex",
              "describe":"content is [hello] hex",
              "content":"68656C6C6F"
            }
          ],
          "md5Items":[
            {
              "name":"hello world md5",
              "describe":"content is [hello] md5",
              "content":"5d41402abc4b2a76b9719d911017c592"
            }
          ],
          "textItems":[
            {
              "name":"hello world text",
              "describe":"content is [world] md5",
              "content":"world"
            }
          ]
        }
	)";

    //bytes
	const string bytes = "fire dog hello world.";

    int ecode = NO_ERROR;

	//step1: init feature library
    FeatureLibrary featureLibrary = FeatureLibrary::createByJson(featureLibraryJson, &ecode);

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
    MatcherFeature* mf = NULL;

    //test md5
    //mf = matcher->matchMd5("5d41402abc4b2a76b9719d911017c592");//found it
    //mf = matcher->matchMd5("11111111111111111111111111111111");//not found

    //test match bytes
    mf = matcher->matchBytes(bytes.c_str(), length);//found it
    //mf = matcher->matchBytes("zzz", strlen("zzz"));//not found

    //test match byte
    //for (int i = 0; i < length; i++) {
    //    char byte = bytes.at(i);
    //    mf = matcher->matchByte(byte);
    //    //found it
    //    if (mf != NULL) {
    //        break;
    //    }
    //}

    if (mf != NULL) {
        cout << "found it:" << mf->name << endl;
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

# 特征库格式 Feature Library Formate

```
{
  "name":"Test Feature Library",
  "version":"1.0.0",
  "author":"MountCloud",
  "createTime":"2021-09-23 00:58:00",
  "hexItems":[
    {
      "name":"hello world hex",
      "describe":"content is [hello] hex",
      "content":"68656C6C6F"
    }
  ],
  "md5Items":[
    {
      "name":"hello world md5",
      "describe":"content is [hello] md5",
      "content":"5d41402abc4b2a76b9719d911017c592"
    }
  ],
  "textItems":[
    {
      "name":"hello world text",
      "describe":"content is [world] md5",
      "content":"world"
    }
  ]
}
```