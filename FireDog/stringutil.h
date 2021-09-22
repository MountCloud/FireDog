#ifndef _FIREDOG_STRINGUTIL_H_
#define _FIREDOG_STRINGUTIL_H_

#include <string> 
#include <vector>
#include <algorithm>
#include <functional> 
#include <sstream>

/// <summary>
/// string util
/// </summary>
class StringUtil {
public:
	static std::string ltrim(std::string str) {
		std::string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
		str.erase(str.begin(), p);
		return str;
	};

	static std::string rtrim(std::string str) {
		std::string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int, int>(isspace)));
		str.erase(p.base(), str.end());
		return str;
	};

	static std::string trim(std::string str) {
		std::string result = ltrim(rtrim(str));
		return result;
	};

	static std::string toUpper(std::string str) {
		transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	static std::string toLower(std::string str) {
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	static bool contains(std::string str, std::string con) {
		std::string::size_type postion = str.find(con);
		if (postion == std::string::npos) {
			return false;
		}
		return true;
	}

	static std::string replaceAll(std::string str, std::string src, std::string dst) // ��org_str �滻Ϊrep_str; 
	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = src.size();
		std::string::size_type dstlen = dst.size();

		while ((pos = str.find(src, pos)) != std::string::npos)
		{
			str.replace(pos, srclen, dst);
			pos += dstlen;
		}
		return str;
	}

	static std::string textToHexText(std::string str) {
		const std::string hex = "0123456789ABCDEF";
		std::stringstream ss;
		for (std::string::size_type i = 0; i < str.size(); ++i)
			ss << hex[(unsigned char)str[i] >> 4] << hex[(unsigned char)str[i] & 0xf];
		return ss.str();
	}

	static void textToBytes(std::string text, std::vector<char>* results) {
		std::string ttext = trim(text);
		std::string hexstr = textToHexText(ttext);
		hexTextToBytes(hexstr, results);
	}
	
	static void hexTextToBytes(std::string hex, std::vector<char>* results) {
		std::string thex = trim(hex);
		thex = replaceAll(thex, " ", "");
		if (thex.size()==0) {
			return;
		}
		if (thex.size() % 2 != 0) {
			return;
		}
		for (size_t i = 0; i < thex.length(); i += 2)
		{
			std::string byte = thex.substr(i, 2);
			char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
			results->push_back(chr);
		}
	}
};


#endif