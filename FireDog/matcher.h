#ifndef _FIREDOG_MATCHER_H_
#define _FIREDOG_MATCHER_H_


#include <string>
#include <vector>
#include <unordered_map>

#include "map/avlmap.h"

/// <summary>
/// fire dog matcher
/// </summary>

namespace mountcloud {
	class RuleData;
}

namespace firedog {
	
	class FeatureLibrary;
	class FeatureLibraryItem;

	class Hex;

	class MatcherFeature {
	public:
		int featureId;
		std::string featureKey;
	};


	class MatcherData {
	public:
		//
		AvlMap<char, MatcherData*>* fullData = NULL;
		//高位
		AvlMap<char, MatcherData*>* highPosData = NULL;
		//低位
		AvlMap<char, MatcherData*>* lowPosData = NULL;
		//任意
		MatcherData* anyData = NULL;

		std::vector<MatcherFeature*>* features = NULL;
		~MatcherData();
	};

	class MatcherDataSource {
	public:
		MatcherDataSource();
		~MatcherDataSource();

		int pushFeatureLibrary(FeatureLibrary* lib);
		MatcherData* getData();
		FeatureLibraryItem* getFeatureLibraryItem(int id);
	private:
		//笛卡尔积 Cartesian product N x N push
		void pushFeature(std::vector<std::vector<Hex*>*>* hexs, int index, MatcherData* data, MatcherFeature* mf);
		MatcherData* data = NULL;

		std::vector<FeatureLibrary*>* librarys = NULL;
		std::unordered_map<int, FeatureLibraryItem*>* libraryItems = NULL;

		int nowLibraryId = 0;
		int createLibraryId();
	};

	class MatcherResult {
	public:
		/// <summary>
		/// featureId by matcher
		/// </summary>
		int featureId;
		/// <summary>
		/// library author
		/// </summary>
		std::string author;

		/// <summary>
		/// name or title
		/// </summary>
		std::string name;

		/// <summary>
		/// describe
		/// </summary>
		std::string describe;
	};

	class Matcher {
	public:
		Matcher(MatcherDataSource* dataSource);
		~Matcher();

		std::vector <MatcherResult*>* matchBytes(const char* bytes, int length);
		std::vector <MatcherResult*>* matchByte(const char byte);
	private:
		/// <summary>
		/// data source,
		/// </summary>
		MatcherDataSource* dataSource = NULL;

		/// <summary>
		/// this is bytes match cache
		/// </summary>
		std::vector<MatcherData*>* matchCache = NULL;

		/// <summary>
		/// Feature Rule Data
		/// </summary>
		std::unordered_map<int, mountcloud::RuleData*>* featureRuleData = NULL;


		std::vector<MatcherResult*>* checkMatcherFeature(MatcherData* mbd, char byte, std::vector<MatcherData*>* tempMatchBytesCache);
		std::vector<MatcherResult*>* checkMatcherFeatureMap(AvlMap<char, MatcherData*>* map,char byte, std::vector<MatcherData*>* tempMatchBytesCache);
	};

}

#endif