// TestWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <iomanip>

#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"
#include "converter.h"

#include "stringutil.h"

#include "json/json-schema/json-schema.hpp"

using namespace std;
using namespace firedog;

using nlohmann::json;
using nlohmann::json_schema::json_validator;

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

    const char* featureLibraryJson_error = "";

    //bytes
	const string bytes = "fire dog hello world.";

    int ecode = NO_ERROR;

	//step1: init feature library
    FeatureLibrary featureLibrary = FeatureLibrary::createByJson(featureLibraryJson_error, &ecode);

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

void converter() {
    string errormsg;
    int errorcode;

    //string hexstr = "68 65 6C 6C 6F";
    //HexData* hexData = Converter::hexTextToBytes(hexstr, &errorcode, &errormsg);
    //delete hexData;
    //hexData = NULL;

    string hexstr = "68 6? [6C-7C] 6C [01-05,07-10] ?? 6F ?7 65";
    HexData* hexData = Converter::hexTextToBytes(hexstr, &errorcode, &errormsg);
    delete hexData;
    hexData = NULL;
}


void testJsonSchema() {
    const string person_schema_json = R"(
        {
            "$schema": "http://json-schema.org/draft-07/schema#",
            "title": "A person",
            "properties": {
                "name": {
                    "description": "Name",
                    "type": "string"
                },
                "age": {
                    "description": "Age of the person",
                    "type": "number",
                    "minimum": 2,
                    "maximum": 200
                }
            },
            "required": [
                         "name",
                         "age"
                         ],
            "type": "object"
        }

    )";

    json person_schema = json::parse(person_schema_json);

    string schema = person_schema["$schema"];

    json_validator validator; // create validator
    try {
        validator.set_root_schema(person_schema); // insert root-schema
    }
    catch (const std::exception& e) {
        std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
        return;
    }

    json bad_person = { {"age", 42} };
    json good_person = { {"name", "Albert"}, {"age", 42} };

    /* json-parse the people - API of 1.0.0, default throwing error handler */

    for (auto& person : { bad_person, good_person }) {
        std::cout << "About to validate this person:\n"
            << std::setw(2) << person << std::endl;
        try {
            validator.validate(person); // validate the document - uses the default throwing error-handler
            std::cout << "Validation succeeded\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Validation failed, here is why: " << e.what() << "\n";
        }
    }

    /* json-parse the people - with custom error handler */
    class custom_error_handler : public nlohmann::json_schema::basic_error_handler
    {
        void error(const nlohmann::json_pointer<nlohmann::basic_json<>>& pointer, const json& instance,
            const std::string& message) override
        {
            nlohmann::json_schema::basic_error_handler::error(pointer, instance, message);
            std::cerr << "ERROR: '" << pointer << "' - '" << instance << "': " << message << "\n";
        }
    };


    for (auto& person : { bad_person, good_person }) {
        std::cout << "About to validate this person:\n"
            << std::setw(2) << person << std::endl;

        custom_error_handler err;
        validator.validate(person, err); // validate the document

        if (err)
            std::cerr << "Validation failed\n";
        else
            std::cout << "Validation succeeded\n";
    }
}

int main()
{
    //testMatcher();
    //converter();
    testJsonSchema();
    return 0;
}