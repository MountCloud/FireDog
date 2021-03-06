#ifndef _MOUNTCOUD_RULE_H_
#define _MOUNTCOUD_RULE_H_

#include <string>
#include <vector>
#include <unordered_map>

#define RULE_BASE_TYPE_ALL		100

#define RULE_BASE_TYPE_LOGIC	200
#define RULE_TYPE_LOGIC_AND		201
#define RULE_TYPE_LOGIC_OR		202
#define RULE_TYPE_LOGIC_NOT		203

#define RULE_BASE_TYPE_NUMBER	300
#define RULE_TYPE_NUMBER_INT	301
#define RULE_TYPE_NUMBER_COUNT	302

#define RULE_BASE_TYPE_COMPARE	400
#define RULE_TYPE_COMPARE_LT	401
#define RULE_TYPE_COMPARE_LE	402
#define RULE_TYPE_COMPARE_GT	403
#define RULE_TYPE_COMPARE_GE	404



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
		virtual bool check(RuleData* data) { return false; };
	public:
		int getBaseType();
		int getType();
	protected:
		int baseType = -1;
		int type = -1;
	};

	class AllRule : public Rule {
	public:
		AllRule(std::string id);
		AllRule(std::vector<std::string> ids);
		void addId(std::string id);
		bool check(RuleData* data) override;
		std::vector<std::string> getIds();
	private:
		std::vector<std::string> ids;
	};


	//逻辑规则
	class LogicRule :public Rule {
	public:
		LogicRule();
		~LogicRule();
		void addRule(Rule* rule);
		void addRules(std::vector<Rule*> rules);
		std::vector<Rule*>* getRules();
	protected:
		std::vector<Rule*>* rules = NULL;
	};

	class AndRule : public LogicRule {
	public:
		AndRule();
		bool check(RuleData* data) override;
	};

	class OrRule : public LogicRule {
	public:
		OrRule();
		bool check(RuleData* data) override;
	};

	class NotRule : public LogicRule {
	public:
		NotRule();
		bool check(RuleData* data) override;
	};

	//数字运算规则
	class NumberRule : public Rule {
	public:
		virtual long getNumber(RuleData* data) { return 0l; };
	public:
		NumberRule();
	};

	class IntRule : public NumberRule {
	public:
		IntRule(long num);
		bool check(RuleData* data) override;
		long getNumber(RuleData* data) override;
		long getNum();
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

		std::vector<std::string> getIds();
	private:
		long critical = 1;
		std::vector<std::string> ids;
	};

	//比较规则
	class CompareRule :public Rule {
	public:
		CompareRule(NumberRule* num1, NumberRule* num2);
		NumberRule* getNum1();
		NumberRule* getNum2();
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