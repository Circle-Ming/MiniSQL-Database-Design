#pragma once

#include "Minisql.h"

/////////////////////////////////////////////////////////////
// Record的辅助函数类(静态)
class RecordUtility
{
public:
	RecordUtility();
	~RecordUtility();

	// 查询某个属性是否存在
	static bool CheckAttriExist(const vector<Attribute> & attrs, const Attribute & attr);

	// 查找某个条件是否有效
	static int CheckConditionExist(const vector<Condition> & cons, const string & attrName);

	// 查找某个记录的索引值
	static int GetIndex(const vector<Attribute> & attrs, const Condition & cons);

	//	查询条件和记录的匹配
	static bool CheckCondition(const vector<Attribute> & attrs, const vector<Condition> & cons, const string * allAttrValues);
	
	//	拷贝内存缓冲区
	static int CopyBuffer(char * des, const char * from, int start, int length);

protected:

	//	查询条件和记录的匹配(单条)
	static bool CheckCondition(const Condition & cond, const string & value, const TYPE type);
};

