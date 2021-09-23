// TestWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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