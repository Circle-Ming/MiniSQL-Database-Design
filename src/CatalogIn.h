#ifndef Catalog__Manager__
#define Catalog__Manager__

#include "MiniSQL.h"
#include "dirent.h"

class CatalogManager
{
	vector<Table> tableList; 
	vector<Index> indexList; 
	int tableNum;

public:
	CatalogManager() {}
	~CatalogManager() {}
	bool API_Catalog(SQLstatement &sql);
	string msg;
	Table* findTable(string tn); 
	Index* findIndex(string in);
	Attribute* getAttribute(Table *t, string an);
	bool createIndex(SQLstatement &sql, Table *t);
	bool clearIndex(Table *t, string in, bool drop);
	void pushBack_indexList(Index &i);
	bool dropIndex(Table *t, Index *i);
	bool dropTable(Table *t);
	bool save_tableInfo(Table *t, bool add);
	void writeAttribute(string fn, Attribute *a);
	void read_tableInfo();
	void update_tableInfo();
	bool save_indexInfo(Index *i, bool add);
	void read_indexInfo();
	void update_indexInfo();
	bool createTable(SQLstatement &sql); 
	bool checkInsert(Table *t, string value); 
	void pushBack_tableList(Table &t); 
	void update_tableNum(bool add); 
	bool checkType(Attribute *a, string v);
	bool checkAttribute(Table *t, vector<Attribute> *a, bool index);
	bool checkCondition(Table *t, vector<Condition> *c);
};

#endif