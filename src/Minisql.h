#ifndef __MINISQL__H__
#define __MINISQL__H__

#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <direct.h>
#include <algorithm>
#include <vector>
#include <map>
#include <list>


using namespace std;

#define FILE_COUNTER 1
#define FILE_INFO_LENGTH 671
#define ATTR_INFO_LENGTH 20
#define FILE_HEAD_LENGTH 31

typedef map< string, vector<string> > AttributeValuesMap;

enum TYPE { MYINT = 0, MYFLOAT = 1, MYCHAR = 2 };
enum RELATION_TYPE { EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, SMALLER, SMALLER_EQUAL, AND, OR};
enum STATEMENT_TYPE { CREATE_DATABASE, CREATE_TABLE, CREATE_INDEX, DROP_TABLE, DROP_INDEX, DROP_DATABASE, SELECT, SELECT_WHERE, INSERT, MYDELETE, DELETE_WHERE, USE, EXECFILE, QUIT, HELP, ILLEGAL,COMMENT};

//�����������ڵ���Ϣ
struct index_info
{
	string index_name;
	int length;
	char type;
	long offset;
	string value;
};

//����������Ϣ
class Attribute
{
public:
	friend class CatalogManager;
	friend class Table;
	string name;
	string indexName;   //index��
	TYPE type;
	int length;
	bool isPrimaryKey;
	bool isUnique;

	Attribute():name(""),indexName(""),type(MYINT),length(0),isPrimaryKey(false),isUnique(false){}
	Attribute(const Attribute & from) :name(from.name), indexName(from.indexName), type(from.type), length(from.length), isPrimaryKey(from.isPrimaryKey), isUnique(from.isUnique){}
	Attribute(string name) :name(name){}
	Attribute(string name, TYPE type, int length, bool isPrimaryKey = false, bool isUnique = false) :name(name), type(type), length(length), isPrimaryKey(isPrimaryKey), isUnique(isUnique){}
	~Attribute(){}
};


//����������Ϣ
class Condition
{
public:
	Attribute attribute;
	string value;
	RELATION_TYPE relationType;

	Condition(){}
	Condition(Attribute attribute, string value, RELATION_TYPE relationType) :attribute(attribute), value(value), relationType(relationType){}
	~Condition(){}
};

//�������Ϣ
class Table
{
public:
	string name;							// ����
	int blockNum;						// ��name.table��ռ�õ�block��
	int recordNum;						// ��¼����
	int attriNum;						// ������
	int tupleLength;					// ÿ����¼�ĳ���
	string primaryKey;						// ��������
	//int freeNum = 0;						// �м�����ɾ���ļ�¼
	vector<Attribute> attributes;			// ָ��Ԫ���������ָ��
	//vector<Attribute>::iterator AttriItor;	// Attribute��iterator
	//vector<string> data;					// ָ�����������ָ��
	//long dataBlockInFile = -1;			// data��ͷ��file�еĿ��λ�ã�ÿ�ű������һ���Ǵ�һ��Ŀ�ͷ��ʼ��
	//vector<string> emptyList;				// ָ��ȴ�ɾ�������ָ�루�ⶫ�����׸����ã�

	Table():blockNum(0),recordNum(0),attriNum(0),tupleLength(0){}
	Table(string name, string primaryKey) :name(name), primaryKey(primaryKey){}
	~Table(){}
};

// �����
class SQLstatement
{
public:
	STATEMENT_TYPE type;
	string tableName;
	string indexName;
	vector<Attribute> attributes;
	vector<Condition> conditions;
	//vector<RELATION_TYPE> relations;
	string content;

	SQLstatement(STATEMENT_TYPE type, string tableName);
	SQLstatement(string SQL);
	~SQLstatement();
	void outputinfo();
	Condition genCondition(string exp);
	RELATION_TYPE getRelationType(string oper);
	STATEMENT_TYPE identify(string type_code);
};

// ����������Ϣ
class Index
{
public:
	string index_name;
	string table_name;
	int startposition;
	int tuplelength;
	int columnLength;
	int IndexOffset;
	Index() :IndexOffset(0){}
};

// ����λ��
class insertPos{
public:
	int bufferNUM;
	int position;
};
#endif