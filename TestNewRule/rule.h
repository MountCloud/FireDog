#ifndef _MOUNTCOUD_RULE_H_
#define _MOUNTCOUD_RULE_H_

#include <string>
#include <vector>
#include <unordered_map>


/// <summary>
/// Lightweight rules engine
/// 轻量级规则引擎
/// 比如这样：{$ors:[$ands:[$count("a",10),$lt($count("b"),$int(10))],$ands:["c","d"]]}
/// </summary>
namespace mountcloud {

	//规则的输入
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


	//规则
	class Rule {
	public:
		virtual bool check(RuleData* data) {};
	};

	class AllRule : public Rule {
	public:
		AllRule(std::string id);
		AllRule(std::vector<std::string> ids);
		void addId(std::string id);
		bool check(RuleData* data) override;
	private:
		std::vector<std::string> ids;
	};


	//逻辑规则
	class LogicRule :public Rule {
	public:
		LogicRule();
		~LogicRule();
		void addRule(Rule* rule);
	protected:
		std::vector<Rule*>* rules = NULL;
	};

	class AndRule : public LogicRule {
	public:
		bool check(RuleData* data) override;
	};

	class OrRule : public LogicRule {
	public:
		bool check(RuleData* data) override;
	};

	class NotRule : public LogicRule {
	public:
		bool check(RuleData* data) override;
	};

	//数字运算规则
	class NumberRule : public Rule {
	public:
		virtual long getNumber(RuleData* data) {};
	};

	class IntRule : public NumberRule {
	public:
		IntRule(long num);
		bool check(RuleData* data) override;
		long getNumber(RuleData* data) override;
	private:
		long num = 0;
	};
	

	//总数规则
	class CountRule : public NumberRule {
	public:
		CountRule(std::string id);
		CountRule(std::vector<std::string> ids);

		void addId(std::string id);
		void addIds(std::vector<std::string> ids);

		long getNumber(RuleData* data) override;
		bool check(RuleData* data) override;

		void setCritical(long critical);
	private:
		long critical = 1;
		std::vector<std::string> ids;
	};

	//比较规则
	class CompareRule :public Rule {
	public:
		CompareRule(NumberRule* num1, NumberRule* num2);
	protected:
		NumberRule* num1 = NULL;
		NumberRule* num2 = NULL;
	};

	class LtRule : public CompareRule {
	public:
		LtRule(NumberRule* num1, NumberRule* num2);

		bool check(RuleData* data) override;
	};

	class LeRule : public CompareRule {
	public:
		LeRule(NumberRule* num1, NumberRule* num2);

		bool check(RuleData* data) override;
	};

	class GtRule : public CompareRule {
	public:
		GtRule(NumberRule* num1, NumberRule* num2);

		bool check(RuleData* data) override;
	};

	class GeRule : public CompareRule {
	public:
		GeRule(NumberRule* num1, NumberRule* num2);

		bool check(RuleData* data) override;
	};

}

#endif