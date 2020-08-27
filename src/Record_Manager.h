#ifndef __RECORD_MANAGER_H__
#define __RECORD_MANAGER_H__

#include "MiniSQL.h"

// Record管理类
class RecordManager
{
public:

	RecordManager(){};
	~RecordManager(){};

public:

	//	插入记录
	bool Insert(Table & table, const string & values, insertPos &ip);

	// 查询记录

	// 按条件查询表中记录
	int SearchByCondition(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions);
	//	无条件查询记录
	int Search(Table & table, const vector<Attribute> & attributes);

	// 删除记录

	//	按照条件删除表中记录
	int  DeleteByCondition(Table & table, vector<Condition> & conditions);	
	//	无条件删除表中记录
	int Delete(Table & table);

	// 其他

	//	删除表格
	bool DeleteTable(string & tableName);

	//	显示记录
	void ShowRecord(int dataCount);

	//	根据不同的数据类型，按照不同的字符串转换方式进行转换
	string GetStringValue(const char * source, int length, TYPE type);

protected:

	//	针对Primary key 和 Nuique Key的查询操作
	bool CheckValid(char * tempData, const Table & table);

	//	查询某个关键字是否存在
	bool CheckExist(char * data, int currentPos, int length, TYPE type, const Table & table);
	
protected:

	map< string, vector<string> > m_Attributes;						// 属性信息列表
};
#endif