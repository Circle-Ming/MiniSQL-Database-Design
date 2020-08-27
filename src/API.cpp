#include "API.h"
#include "CatalogIn.h"
#include "Interpreter.h"
#include "mybuffer.h"
#include "Record_Manager.h"
#include "Index.h"
#include<afx.h>

extern CatalogManager cm;
extern RecordManager rm;
extern Index im;
extern buffer bm;
extern bool quit;

string API::read()
{
	string SQL;
	string temp;
	bool finish = false;
	SQL = "";
	while (!finish)
	{
		cin >> temp;
		SQL = SQL + ' ' + temp;
		if (SQL.at(SQL.length() - 1) == ';') {
			SQL.erase(0, 1);
			finish = true;
		}
	}
	return SQL;
}

string API::down(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}
void API::APIM(SQLstatement &s)
{
	if (s.type == CREATE_TABLE) {
		if (cm.API_Catalog(s)) {
			cout << endl;
		}
		else {
			cout << "create table failed\n" << endl;
		}
	}
	else if (s.type == CREATE_INDEX) {
		if (cm.API_Catalog(s)) {
			Table *t = cm.findTable(s.tableName);
			Attribute *a = NULL;
			vector<Attribute>::iterator iter;
			for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++) {
				if (iter->name == s.attributes.data()->name)
				{
					a = &(*iter);
				}
			}
			im.Create_Index(s.indexName, *a);
			cout << s.indexName << endl;
		}
		else {
			cout << "create index failed\n" << endl;
		}
	}
	else if (s.type == DROP_TABLE) {
		if (cm.API_Catalog(s)) {
			rm.dropTable(cm.msg);
			cout << endl;
		}
		else {
			cout << "drop table failed\n" << endl;
		}
	}
	else if (s.type == DROP_INDEX) {
		if (cm.API_Catalog(s)) {
			// µ÷index
			im.Drop_Index(s.indexName);
			cout << endl;
		}
		else {
			cout << "drop index failed\n" << endl;
		}
	}
	else if (s.type == SELECT) {
		if (cm.API_Catalog(s)) {
			int resultCount = rm.selectWithoutwhere(*cm.findTable(s.tableName), s.attributes);
			if (resultCount)
			{
				rm.outputMap(resultCount);
				cout << resultCount << " row(s) selected:\n" << endl;
			}
			else
			{
				cout << "no match row\n" << endl;
			}
		}
		else {
			cout << "select failed.\n" << endl;
		}
	}
}

void CatalogManager::read_indexInfo()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./cm/index/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string fn(ent->d_name);
			ifstream fin("./cm/index/" + fn, ios::_Nocreate);
			if (fin) {
				Index *i = new Index();
				string s;
				getline(fin, s);
				if (s == "") {
					cout << "wrong file." << endl;
					exit(0);
				}
				getline(fin, s);
				i->columnLength = stoi(s);
				getline(fin, s);
				i->IndexOffset = stoi(s);
				getline(fin, i->index_name);
				getline(fin, s);
				i->startposition = stoi(s);
				getline(fin, i->table_name);
				getline(fin, s);
				i->tuplelength = stoi(s);
				indexList.push_back(*i);
			}
		}
		closedir(dir);
	}
	else {
		cout << "directory ./cm/index/ not exist." << endl;
	}
}