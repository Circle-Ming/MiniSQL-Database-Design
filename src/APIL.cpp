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

void API::APIL(SQLstatement &s)
{
	if (s.type == SELECT_WHERE) {
		if (cm.API_Catalog(s)) {
			Table *t = cm.findTable(s.tableName);
			Index *i;
			if (0) {
				Attribute *a = cm.getAttributebyi(t, i->index_name);
				insertPos ip;
				string value = "";
				vector<Attribute>::iterator iter;
				if (im.Find_Index(*a, s.conditions[0].value, ip.bufferNUM, ip.position)) {
					if (s.attributes[0].name == "*") {
						for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++) {
							if (iter->name.length()>7) {
								if (iter->name.length() > 15)
									cout << iter->name << "\t";
								else
									cout << iter->name << "\t\t\t";
							}
							else {
								cout << iter->name << "\t\t\t\t\t";
							}
						}
						cout << endl;
						vector<Attribute>::iterator iter2;
						int l = 1;
						for (iter2 = t->attributes.begin(); iter2 != t->attributes.end(); iter2++) {
							value = rm.toString(bm.bufferBlock[ip.bufferNUM].value + ip.position + l, iter2->length, iter2->type);
							if (value.length()>7) {
								if (value.length() > 15)
									cout << value << "\t";
								else
									cout << value << "\t\t";
							}
							else {
								cout << value << "\t\t\t\t";
							}
							l += iter2->length;
						}
					}
					else {
						for (iter = s.attributes.begin(); iter != s.attributes.end(); iter++) {
							if (iter->name.length()>7) {
								if (iter->name.length() > 15)
									cout << iter->name << "\t";
								else
									cout << iter->name << "\t\t\t";
							}
							else {
								cout << iter->name << "\t\t\t";
							}
						}
						cout << endl;
						int l = 1;
						for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++) {
							vector<Attribute>::iterator iter2;
							for (iter2 = s.attributes.begin(); iter2 != s.attributes.end(); iter2++) {
								if (iter->name == iter2->name) {
									value = rm.toString(bm.bufferBlock[ip.bufferNUM].value + ip.position + l, iter->length, iter->type);
									if (value.length()>7) {
										if (value.length() > 15)
											cout << value << "\t";
										else
											cout << value << "\t\t";
									}
									else {
										cout << value << "\t\t\t";
									}
									break;
								}
							}
							l += iter->length;
						}
					}
					cout << endl << "1 row seleted." << endl;
					return;
				}
			}
			else {
				int resultCount = rm.selectWithwhere(*cm.findTable(s.tableName), s.attributes, s.conditions);
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
		}
		else {
			cout << "select failed.\n" << endl;
		}
	}
	else if (s.type == INSERT) {
		if (cm.API_Catalog(s)) {
			insertPos ip;
			if (rm.insertValue(*cm.findTable(s.tableName), s.content, ip))
			{
				cout << "Insert Success\n" << endl;

				im.Insert_Index(cm.findTable(s.tableName), s.content, ip.bufferNUM, ip.position);
			}
			else
			{
				cout << "insert failed.\n" << endl;
			}
		}
		else {
			cout << "insert failed.\n" << endl;
		}
	}
	else if (s.type == MYDELETE) {
		if (cm.API_Catalog(s)) {
			int deleteCount = rm.deleteWithoutwhere(*cm.findTable(s.tableName));
			if (deleteCount)
			{
				cout << deleteCount << " row(s) deleted\n" << endl;
			}
			else
			{
				cout << "no row deleted\n" << endl;
			}
		}
		else {
			cout << "delete failed.\n" << endl;
		}
	}
	else if (s.type == DELETE_WHERE) {
		if (cm.API_Catalog(s)) {
			int deleteCount = rm.deleteWithwhere(*cm.findTable(s.tableName), s.conditions);
			if (deleteCount)
			{
				cout << deleteCount << " row(s) deleted\n" << endl;
			}
			else
			{
				cout << "no row deleted\n" << endl;
			}
		}
		else {
			cout << "delete failed.\n" << endl;
		}
	}
	else if (s.type == EXECFILE) {
		fstream fin(s.tableName);
		if (fin) {
			string sql;
			string temp;
			bool finish = false;
			sql = "";
			while (fin >> temp) {
				sql = sql + ' ' + temp;
				if (sql.at(sql.length() - 1) == ';') {
					sql.erase(0, 1);
					cout << sql << endl;
					sql = Interpreter(sql);
					SQLstatement s(sql);
					API(s);
					sql = "";
				}
			}
		}
		else {
			cout << "no such file.\n" << endl;
		}
	}
	else if (s.type == QUIT) {
		quitFlag = true;
		cm.update_tableInfo();
		cm.update_indexInfo();
	}
	else if (s.type == HELP) {
		getHelp();
	}
	else if (s.type == ILLEGAL) {
		cout << "wrong SQL statement." << endl;
	}
	else if (s.type == COMMENT);
}

void CatalogManager::update_indexInfo()
{
	bool add = true;
	vector<Index>::iterator iter;
	for (iter = indexList.begin(); iter != indexList.end(); iter++) {
		save_indexInfo(&(*iter), add);
	}
}

Attribute* CatalogManager::getAttribute(Table *t, string an)
{
	vector<Attribute>::iterator iter;
	for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++) {
		if (iter->indexName == an) {
			return &(*iter);
		}
	}
	return NULL;
}