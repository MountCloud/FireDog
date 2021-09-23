#ifndef _FIREDOG_MATCHER_H_
#define _FIREDOG_MATCHER_H_


#include <string>
#include <vector>
#include <unordered_map>

#include "map/avlmap.h"

/// <summary>
/// fire dog matcher
/// </summary>
namespace firedog {
	
	class FeatureLibrary;
	class FeatureLibraryItem;

	class MatcherFeature {
	public:
		/// <summary>
		/// name or title
		/// </summary>
		std::string name;

		/// <summary>
		/// author
		/// </summary>
		std::string author;

		/// <summary>
		/// describe
		/// </summary>
		std::string describe;

		/// <summary>
		/// feature content,hex or md5 or text
		/// </summary>
		std::string content;

		/// <summary>
		/// type,see featurelibrary.h #define FEATURE_TYPE_xxx
		/// </summary>
		int type;

		~MatcherFeature();
	};

	class MatcherMd5Data {
	public:
		std::unordered_map<std::string, MatcherFeature*>* data = NULL;
		~MatcherMd5Data();
	};

	class MatcherByteData {
	public:
		AvlMap<char, MatcherByteData*>* data = NULL;
		MatcherFeature* feature = NULL;
		~MatcherByteData();
	};

	class MatcherDataSource {
	public:
		MatcherDataSource();

		MatcherMd5Data* md5Data = NULL;
		MatcherByteData* byteData = NULL;

		int pushFeatureLibrary(FeatureLibrary lib);

		std::vector<FeatureLibrary>* librarys;

		~MatcherDataSource();

	private:
		MatcherFeature* toMatcherFeature(FeatureLibraryItem item,int type);
		void pushByteFeature(std::vector<char>* bytes, MatcherFeature* mf);
	};

	class Matcher {
	public:
		Matcher(MatcherDataSource* dataSource);
		~Matcher();

		MatcherFeature* matchMd5(std::string md5);

		MatcherFeature* matchBytes(const char* bytes, int length);

		MatcherFeature* matchByte(const char byte);
	private:
		/// <summary>
		/// data source,
		/// </summary>
		MatcherDataSource* dataSource = NULL;

		/// <summary>
		/// this is bytes match cache
		/// </summary>
		std::vector<MatcherByteData*>* matchBytesCache = NULL;
	};

}

#endif