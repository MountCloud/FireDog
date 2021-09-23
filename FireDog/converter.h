#ifndef _FIREDOG_CONVERTER_H_
#define _FIREDOG_CONVERTER_H_

#include <vector>
#include <string>


#define HEX_DATA_TYPE_FULL	1
#define HEX_DATA_TYPE_HIGH	2
#define HEX_DATA_TYPE_LOW	3
#define HEX_DATA_TYPE_ANY	4

namespace firedog {

	class Hex {
	public:
		int type;
		char data;
	};

	class HexData {
	public:
		std::vector<std::vector<Hex*>*>* hexs = NULL;

		HexData();
		~HexData();
	};

	class Converter {
	public:
		static HexData* hexTextToBytes(std::string hextext,int* errorcode,std::string* errormessage);
		static HexData* textToBytes(std::string text, int* errorcode, std::string* errormessag);
	private:
		static void free(std::vector<Hex*>* hexlist, HexData* hexdata);
		static int hexTextFormateCheck(std::string hextext, std::string* errormessag);
	};

}

#endif