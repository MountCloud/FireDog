#ifndef _MOUNTCOUD_RULE_H_
#define _MOUNTCOUD_RULE_H_

#include <string>
#include <vector>
#include <unordered_map>


/// <summary>
/// Lightweight rules engine
/// 轻量级规则引擎
/// 比如这样：{$ors:[$ands:["a","b"],$ands:["c","d"]]}
/// </summary>
namespace mountcloud {
	class RuleData {
	public:
		RuleData();
		~RuleData();
		void hit(std::string id);
		bool has(std::string id);
		long count(std::string id);
	private:
		std::unordered_map<std::string, long>* data = NULL;
	};

	class Rule {
	public:
		Rule();
		~Rule();
		void addRule(Rule* rule);
	public:
		virtual bool check(RuleData* data);
	protected:
		std::vector<Rule*>* rules = NULL;
	};
	
	class AndRule : public Rule{
	public:
		bool check(RuleData* data) override;
	};

	class OrRule : public Rule {
	public:
		bool check(RuleData* data) override;
	};

	//class CountRule : public Rule {
	//public:
	//private:
	//	int 
	//};

	//{$ors:[$ands:["a","b"],$ands:["c","d"]]}
	//class Rule {
	//public:
	//	std::vector<Rule*>* ands = NULL;
	//	std::vector<Rule*>* ors = NULL;
	//	std::string id;
	//	bool check(RuleData* data);
	//	~Rule();
	//};

}

#endif