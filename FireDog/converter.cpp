#include "converter.h"

#include "stringutil.h"
#include "errorcode.h"

using namespace std;
using namespace firedog;

HexData::HexData() {
	hexs = new vector<vector<Hex*>*>();
}

HexData::~HexData() {
	if (hexs != NULL) {
		for (int i = 0; i < hexs->size(); i++) {
			vector<Hex*>* tphexs = hexs->at(i);
			if (tphexs != NULL) {
				for (int y = 0; y < tphexs->size(); y++) {
					Hex* hex = tphexs->at(y);
					delete hex;
					hex = NULL;
				}
				delete tphexs;
				tphexs = NULL;
			}
		}
		delete hexs;
		hexs = NULL;
	}
}

HexData* Converter::hexTextToBytes(string hextext, int* errorcode, string* errormessag) {
	HexData* result = new HexData();

	string tempHexText = StringUtil::replaceAll(hextext, " ", "");
	tempHexText = StringUtil::trim(tempHexText);
	tempHexText = StringUtil::toUpper(tempHexText);

	int laststartindex = 0;

	//普通的用
	char tempchar[2][3];
	tempchar[0][2] = 0;
	tempchar[1][2] = 0;

	bool isInRange = false;
	bool isRangeEnd = false;


	vector<Hex*>* oneHexList = new vector<Hex*>();

	for (int i = 0; i < tempHexText.size(); i++) {
		char c = tempHexText.at(i);

		if (c == '[') {
			isInRange = true;
			laststartindex = i + 1;
			continue;
		}

		//结尾符号放在最后判断
		if (c == ']') {
			isInRange = false;
			isRangeEnd = false;
			laststartindex = i + 1;
			result->hexs->push_back(oneHexList);
			oneHexList = new vector<Hex*>();
			continue;
		}


		if (c == '-') {
			isRangeEnd = true;
			laststartindex = i + 1;
			continue;
		}

		if (c == ',') {
			isRangeEnd = false;
			laststartindex = i + 1;
			continue;
		}

		//处理方法
		if ((i - laststartindex) % 2 == 0) {
			if (!isRangeEnd) {
				tempchar[0][0] = c;
			}
			else {
				tempchar[1][0] = c;
			}
		}
		else {
			if (!isRangeEnd) {
				tempchar[0][1] = c;
			}
			else {
				tempchar[1][1] = c;
			}
			
			//在区间中并且没有结束，继续遍历
			if (isInRange && !isRangeEnd) {
				continue;
			}
			//在区间中并且结束了
			else if (isInRange && isRangeEnd) {
				char startbyte = (char)(int)strtol(tempchar[0], NULL, 16);
				char endbyte = (char)(int)strtol(tempchar[1], NULL, 16);
				if (startbyte > endbyte) {
					//这个是有问题的，start大于end了
					errormessag->append("hex range formate error,start less than end:");
					errormessag->append(tempchar[0]);
					errormessag->append("-");
					errormessag->append(tempchar[1]);
					*errorcode = C_CONTENT_FORMATE_ERROR;
					free(oneHexList, result);
					return NULL;
				}
				//只有一个
				if (startbyte == endbyte) {
					Hex* hex = new Hex();
					//这个是普通的hex
					hex->type = HEX_DATA_TYPE_FULL;
					hex->data = startbyte;
					oneHexList->push_back(hex);
				}else{
					for (char byte = startbyte; byte <= endbyte; byte++) {
						Hex* hex = new Hex();
						//这个是普通的hex
						hex->type = HEX_DATA_TYPE_FULL;
						hex->data = byte;
						oneHexList->push_back(hex);
					}
				}
				//如果是结尾符
				if (c==']') {
					result->hexs->push_back(oneHexList);
					oneHexList = new vector<Hex*>();
				}
			}
			else {
				//非区间就一个byte，根据情况添加
				Hex* hex = new Hex();
				//只有高位
				if (tempchar[0][0] != '?' && tempchar[0][1] == '?') {
					char tempcs[1] = { tempchar[0][0] };
					char byte = (char)(int)strtol(tempcs, NULL, 16);
					hex->type = HEX_DATA_TYPE_HIGH;
					hex->data = byte;
				}
				//只有低位
				else if (tempchar[0][0] == '?' && tempchar[0][1] != '?') {
					char tempcs[1] = { tempchar[0][1] };
					char byte = (char)(int)strtol(tempcs, NULL, 16);
					hex->type = HEX_DATA_TYPE_LOW;
					hex->data = byte;
				}
				//any
				else if (tempchar[0][0] == '?' && tempchar[0][1] == '?') {
					hex->type = HEX_DATA_TYPE_ANY;
				}
				else {
					//这个是普通的hex
					char byte = (char)(int)strtol(tempchar[0], NULL, 16);
					hex->type = HEX_DATA_TYPE_FULL;
					hex->data = byte;
				}

				oneHexList->push_back(hex);
				result->hexs->push_back(oneHexList);
				oneHexList = new vector<Hex*>();
			}
		}
	}
	*errorcode = NO_ERROR;
	return result;
}


HexData* Converter::textToBytes(string text, int* errorcode, string* errormessag) {
	HexData* result = NULL;
	vector<char> bytes;
	StringUtil::textToBytes(text, &bytes);
	if (bytes.size() > 0) {
		result = new HexData();
		for (int i = 0; i < bytes.size(); i++) {
			char byte = bytes[i];
			Hex* hex = new Hex();
			hex->data = byte;
			hex->type = HEX_DATA_TYPE_FULL;

			vector<Hex*>* hexs = new vector<Hex*>();
			hexs->push_back(hex);
			result->hexs->push_back(hexs);
		}
		*errorcode = NO_ERROR;
	}
	else {
		*errorcode = C_CONTENT_FORMATE_ERROR;
	}
	return result;
}

void Converter::free(vector<Hex*>* hexlist, HexData* hexdata){
	if (hexlist != NULL) {
		for (int i = 0; i < hexlist->size(); i++) {
			Hex* hex = hexlist->at(i);
			delete hex;
			hex = NULL;
		}
		delete hexlist;
		hexlist = NULL;
	}
	if (hexdata != NULL) {
		delete hexdata;
		hexdata = NULL;
	}
}

int Converter::hexTextFormateCheck(string hextext, std::string* errormessag) {
	return true;
}