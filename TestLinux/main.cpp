#include <cstdio>
#include <iostream>
#include <string>
#include <iomanip>

#include "firedog.h"
#include "featurelibrary.h"
#include "matcher.h"

#include "stringutil.h"

#include "json/json-schema/json-schema.hpp"

#include "rule/rule.h"

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

    //bytes
    const string bytes = "fire dog hello world.";

    int ecode = NO_ERROR;

    //step1: init feature library
    FeatureLibrary featureLibrary = *FeatureLibrary::createByJson(featureLibraryJson, &ecode);

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
    //mf = matcher->matchBytes("zzz", 3);//not found

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
		        },
		        "content": {
			        "description": "hex content",
			        "type": "string",
                    "pattern":"^(([a-fA-F0-9?]{2}[ ]?)|(\\[([0-9a-fA-F]{2}[ ]?-[ ]?[a-fA-F0-9]{2}[ ]?[,]?){1,}\\][ ]?)){0,}$"
		        }
	        },
	        "required": [
		        "name",
		        "age",
		        "content"
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

    string good_person_json = R"(
        {
	        "name": "Albert",
	        "age": 42,
	        "content": "A1 5C AE ?B [01-05] [0A-A0,07-0A] C? ?? BBb"
        }
    )";

    json good_person = json::parse(good_person_json);


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

void testRule() {
    mountcloud::RuleData* data = new mountcloud::RuleData();
    mountcloud::Rule* rule = new mountcloud::Rule();

    //  ((a && b) || (c && d))
    rule->ors = new std::vector<mountcloud::Rule*>();

    mountcloud::Rule* a_and_b_rule = new mountcloud::Rule();
    a_and_b_rule->ids = new vector<string>();
    a_and_b_rule->ids->push_back("a");
    a_and_b_rule->ids->push_back("b");

    mountcloud::Rule* c_and_d_rule = new mountcloud::Rule();
    c_and_d_rule->ids = new vector<string>();
    c_and_d_rule->ids->push_back("c");
    c_and_d_rule->ids->push_back("d");

    rule->ors->push_back(a_and_b_rule);
    rule->ors->push_back(c_and_d_rule);

    bool state = rule->check(data);
    cout << state << endl;

    data->set("a", true);
    state = rule->check(data);
    cout << state << endl;

    data->set("b", true);
    state = rule->check(data);
    cout << state << endl;

    data->set("a", false);
    data->set("c", true);
    state = rule->check(data);
    cout << state << endl;

    data->set("d", true);
    state = rule->check(data);
    cout << state << endl;
}

int main()
{
    //testMatcher();
    //estJsonSchema();
    testRule();
    return 0;
}