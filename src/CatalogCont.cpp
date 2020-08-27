#include "CatalogIn.h"
#include "Index.h"

extern IndexManager im;

Table* CatalogManager::findTable(string tn)
{
	vector<Table> ::iterator iter;
	for (iter = tableList.begin(); iter != tableList.end(); iter++)
	{
		if (iter->name == tn)
		{
			return &(*iter);
		}
	}
	return NULL;
}

bool CatalogManager::save_indexInfo(Index *i, bool add)
{
	if (add) {
		ofstream fout;
		fout.open("./cm/index/" + i->index_name + "." + i->table_name, ios::trunc);
		if (fout) {
			fout << ";" << endl;
			fout << i->columnLength << endl;
			fout << i->IndexOffset << endl;
			fout << i->index_name << endl;
			fout << i->startposition << endl;
			fout << i->table_name << endl;
			fout << i->tuplelength << endl;
			fout.close();
			return true;
		}
		else {
			cout << "open file failed." << endl;
			fout.close();
			return false;
		}
	}
	else {
		if (remove(("./cm/index/" + i->index_name + "." + i->table_name).c_str())) {
			cout << i->index_name << " not exist." << endl;
			return false;
		}
		else {
			cout << i->index_name << " removed." << endl;
			return true;
		}
	}
}
bool CatalogManager::API_Catalog(SQLstatement &sql)
{
	if (sql.type == CREATE_TABLE) {
		if (findTable(sql.tableName)) {
			cout << sql.tableName << " existed." << endl;
			return false;
		}
		else {
			if (createTable(sql)) {
				// µ÷index
				cout << sql.tableName << " created successfully." << endl;
				return true;
			}
			else {
				return false;
			}
		}
	}
	else if (sql.type == CREATE_INDEX) {
		bool index = true;
		Table *t = findTable(sql.tableName);
		if (t) {
			if (checkAttribute(t, &sql.attributes, index)) {
				if (findIndex(sql.indexName)) {
					cout << sql.indexName << " existed." << endl;
					return false;
				}
				else {
					if (createIndex(sql, t)) {
						cout << sql.indexName << " created successfully." << endl;
						return true;
					}
					else {
						return true;
					}
				}
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DROP_TABLE) {
		Table *t = findTable(sql.tableName);
		if (t) {
			// µ÷index¡¢record
			im.Drop_Table_Index(t);
			msg = t->name;
			if (dropTable(t)) {
				cout << msg << " dropped successfully." << endl;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DROP_INDEX) {
		Index *i;
		if (i = findIndex(sql.indexName)) {
			Table *t = findTable(i->table_name);
			if (dropIndex(t, i)) {
				return true;
			}
			return false;
		}
		else {
			cout << sql.indexName << " not exist." << endl;
			return false;
		}

	}
	else if (sql.type == SELECT) {
		bool index = false;
		Table *t = findTable(sql.tableName);
		if (t) {
			if (checkAttribute(t, &sql.attributes, index)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == SELECT_WHERE) {
		bool index = false;
		Table *t = findTable(sql.tableName);
		if (t) {
			if (checkAttribute(t, &sql.attributes, index) && checkCondition(t, &sql.conditions)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == INSERT) {
		Table *t = findTable(sql.tableName);
		if (t) {
			if (checkInsert(t, sql.content)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == MYDELETE) {
		Table *t = findTable(sql.tableName);
		if (t) {
			return true;
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DELETE_WHERE) {
		Table *t = findTable(sql.tableName);
		if (t) {
			if (checkCondition(t, &sql.conditions)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
}

