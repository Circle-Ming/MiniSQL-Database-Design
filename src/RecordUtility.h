#pragma once

#include "Minisql.h"

/////////////////////////////////////////////////////////////
// Record�ĸ���������(��̬)
class RecordUtility
{
public:
	RecordUtility();
	~RecordUtility();

	// ��ѯĳ�������Ƿ����
	static bool CheckAttriExist(const vector<Attribute> & attrs, const Attribute & attr);

	// ����ĳ�������Ƿ���Ч
	static int CheckConditionExist(const vector<Condition> & cons, const string & attrName);

	// ����ĳ����¼������ֵ
	static int GetIndex(const vector<Attribute> & attrs, const Condition & cons);

	//	��ѯ�����ͼ�¼��ƥ��
	static bool CheckCondition(const vector<Attribute> & attrs, const vector<Condition> & cons, const string * allAttrValues);
	
	//	�����ڴ滺����
	static int CopyBuffer(char * des, const char * from, int start, int length);

protected:

	//	��ѯ�����ͼ�¼��ƥ��(����)
	static bool CheckCondition(const Condition & cond, const string & value, const TYPE type);
};

