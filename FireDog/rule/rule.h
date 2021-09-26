#ifndef _MOUNTCOUD_RULE_H_
#define _MOUNTCOUD_RULE_H_

#include <string>
#include <vector>
#include <unordered_map>


/// <summary>
/// Lightweight rules engine
/// ��������������
/// ����������{$ors:[$ands:["a","b"],$ands:["c","d"]]}
/// </summary>
namespace mountcloud {

	class RuleData {
	public:
		RuleData();
		~RuleData();
		void set(std::string id, bool state);
		bool get(std::string id);
	private:
		std::unordered_map<std::string, bool>* data = NULL;
	};
	
	//{$ors:[$ands:["a","b"],$ands:["c","d"]]}
	class Rule {
	public:
		std::vector<Rule*>* ands = NULL;
		std::vector<Rule*>* ors = NULL;
		std::string id;
		bool check(RuleData* data);
		~Rule();
	};

}

#endif