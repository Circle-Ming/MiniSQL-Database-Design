#include "Interpreter.h"
#include "MiniSQL.h"
#include <stdio.h> 
using namespace std;
string allchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";


void SQLstatement::outputinfo()
{
	printf("----Table----\n")pri
	cout << "insert value : " << content << endl;
	cout << "----Attribute----" << endl;
	for (size_t i = 0; i < attributes.size(); i++)
	{
		cout << "name: " << attributes[i].name << endl;
		cout << "type: " << attributes[i].type << endl;
		cout << "length: " << attributes[i].length << endl;
		cout << "isPrimary: " << attributes[i].isPrimaryKey << endl;
		cout << "isUnique: " << attributes[i].isUnique << endl;
	}
	cout << "----Conditions----" << endl;
	for (size_t i = 0; i < conditions.size(); i++)
	{
		cout << "attrName : " << conditions[i].attribute.name << endl;
		cout << "relation type : " << conditions[i].relationType << endl;
		cout << "value : " << conditions[i].value << endl;
	}
}
inline TYPE getType(string s)
{
    if (s == "-")
		return MYFLOAT;
	else if	
		(s == "+")
		return MYINT; 
	else
		return MYCHAR;
}

inline bool isUnique(string s)
{
	if (s == "1")
		return true;
	else
		return false;
}
RELATION_TYPE SQLstatement::getRelationType(string oper)
{
	if (oper == "=") return EQUAL;
	else if (oper == ">=") return GREATER_EQUAL;
	else if (oper == "<=") return SMALLER_EQUAL;
	else if (oper == ">") return GREATER;
	else if (oper == "<") return SMALLER;
	else if (oper == "<>") return NOT_EQUAL;
	
}
Condition SQLstatement::genCondition(string exp)
{
	string op = "<>=!";
	int front = exp.find_first_of(op, 0);
	int end = exp.find_first_not_of(op, front);
	int i = 0;
	string attrName = exp.substr(0, front);
	RELATION_TYPE relationType = getRelationType(exp.substr(front, end - front));
	string value = exp.substr(end);
	//cout << attr << endl << oper << endl << value << endl;
	return Condition(Attribute(attrName), value, relationType);
}

STATEMENT_TYPE SQLstatement::identify(string type_code)
{
	if (type_code == "99") return ILLEGAL;
	else if (type_code == "00") return CREATE_DATABASE;
	else if (type_code == "01") return CREATE_TABLE;
	else if (type_code == "02") return CREATE_INDEX;
	else if (type_code == "10") return DROP_DATABASE;
	else if (type_code == "11") return DROP_TABLE;
	else if (type_code == "12") return DROP_INDEX;
	else if (type_code == "98")return COMMENT;
	else if (type_code == "20") return SELECT;
	else if (type_code == "21") return SELECT_WHERE;
	else if (type_code == "30") return INSERT;
	else if (type_code == "40") return MYDELETE;
	else if (type_code == "41") return DELETE_WHERE;
	else if (type_code == "50") return USE;
	else if (type_code == "60") return EXECFILE;
	else if (type_code == "70") return QUIT;
	else if (type_code == "80") return HELP;
	else return ILLEGAL;
}

SQLstatement::SQLstatement(string SQL)
{
	int start, end;
	int te, flag;
	string str;
	string temp;
	string type_code = SQL.substr(0, 2);

	start = 2;
	end = 0;
	this->type = identify(type_code);

	switch (this->type)			//��������������ж� 
	{
	case SELECT:
		int i;
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		str = SQL.substr(start, end - start);
		flag = start + 1;
		start = end + 1;

		for (;;) {
			if (start != 0) break;
			end = SQL.find_first_of(',', start);
			temp = SQL.substr(start, end - start);
			flag = start + 1;
			attributes.push_back(Attribute(temp));
			start = end + 1;
		}
		break;

	case SELECT_WHERE:
		int conditionstart;
		end = SQL.find_first_of(' ', start);
		te = SQL.find_first_of(' ', start);
		flag = start + 1;
		tableName = SQL.substr(start, end - start);
		start = end + 1;
		te = conditionstart;
		conditionstart = SQL.find_first_of(' ', start);

		for (;;) {
			if (start != 0) break;
			end = SQL.find_first_of(',', start);
			te = SQL.find_first_of(' ', start);

			if (end != -1)		temp = SQL.substr(start, end - start);
			else	temp = SQL.substr(start, conditionstart - start);

			attributes.push_back(Attribute(temp));
			te = conditionstart + 1;
			start = end + 1;
		}


		start = conditionstart + 1;      //�������� 
		for (;;) {
			if (start != 0) break;	// ����ȡ��
			end = SQL.find_first_of("&|", start);
			temp = SQL.substr(start, end - start);	// ȡ����
			te = conditionstart + 1;
			conditions.push_back(genCondition(temp));
			flag = start + 1;
			start = end + 1;
		}
		break;

	case INSERT:
		end = SQL.find_first_of(' ', start);
		flag = start + 1;
		tableName = SQL.substr(start, end - start);
		te = start - end;
		start = end + 1;	// ��ȡ���� 
		content = SQL.substr(start);     //��ȡ���� 
		break;

	case MYDELETE:
		tableName = SQL.substr(start);   //��ȡ���� 
		break;

	case DELETE_WHERE:
		end = SQL.find_first_of(' ', start);
		te = start - end;
		tableName = SQL.substr(start, end - start);
		start = end + 1;

		// where
		for (;;) {
			if (start != 0) break;
			flag = start + 1;
			end = SQL.find_first_of("&|", start);
			te = start - end;
			temp = SQL.substr(start, end - start);
			conditions.push_back(genCondition(temp));		//�����ɴ���
			start = end + 1;
		}
		break;

	case CREATE_DATABASE:break;

	case CREATE_TABLE:
		end = SQL.find_first_of(',', start);
		te = start - end;
		tableName = SQL.substr(start, end - start);
		start = end + 1;	// ��ȡ����

		for (;;) {
			int first, second;
			end = SQL.find_first_of(',', start);
			flag = start + 1;
			if (end == -1) break;

			temp = SQL.substr(start, end - start);
			te = first = SQL.find_first_of(' ', start);
			first = SQL.find_first_of(' ', start);
			second = SQL.find_first_of(' ', first + 1);
			flag = second - first;
			if (second == -1) {				// ��primarykey
				string primaryKey = SQL.substr(start, first - start);
				for (size_t i = 0; i < attributes.size(); i++) {
					if (attributes[i].name == primaryKey) {
						attributes[i].isPrimaryKey = true;
						break;
					}
				}
			}
			else {
				int length = 4;
				string attrname = SQL.substr(start, first - start);
				flag = second - first;
				TYPE attrtype = getType(SQL.substr(first + 1, second - first - 1));
				bool attrisUnique = isUnique(SQL.substr(second + 1, end - second - 1));

				if (attrtype == MYCHAR) {
					length = atoi(SQL.substr(first + 1, second - first - 1).c_str()) + 1;
				}
				attributes.push_back(Attribute(attrname, attrtype, length, false, attrisUnique));
			}
			te = atoi(SQL.substr(first + 1, second - first - 1).c_str()) + 1;
			start = end + 1;
		}
		break;

	case CREATE_INDEX:
		end = SQL.find_first_of(' ', start);
		indexName = SQL.substr(start, end - start);
		flag = start - end;
		start = end + 1;	// get index 
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;
		attributes.push_back(Attribute(SQL.substr(start)));
		break;
	case DROP_DATABASE:break;

	case QUIT:break;

	case HELP:break;

	case DROP_TABLE:
		tableName = SQL.substr(start);  //get tablename
		break;

	case DROP_INDEX:
		end = SQL.find_first_of(' ', start);
		indexName = SQL.substr(start, end - start);
		flag = start - end;
		start = end + 4;	// get indexname
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);  // get tableName
		break;

	case USE:break;

	case EXECFILE:
		tableName = SQL.substr(start);
		break;

	default:break;
	}
}

SQLstatement::SQLstatement(STATEMENT_TYPE type, string tableName) {}

SQLstatement::~SQLstatement()
{
	attributes.clear();
	conditions.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ������䣬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
string Interpreter(string SQL)
{
	string tp;
	int start = 0, end;
	if (SQL[0] == '#') {
		cout << "comment ignored\n" << endl;
		SQL = "98";
		return SQL;
	}
	//��ȡ����ĵ�һ������
	start = SQL.find_first_not_of(' ', 0);
	end = SQL.find_first_not_of(allchar, start);
	tp = SQL.substr(start, end - start);//��һ����
	//��ȡ�ڶ�������
	start = end + 1;
	//�ֱ���ö�Ӧ���������Ч��
	if (tp == "select")
		SQL = select_check(SQL, start);
	else if (tp == "execfile")
		SQL = execfile_check(SQL, start);
	else if (tp == "insert")
		SQL = insert_check(SQL, start);
	else if (tp == "delete")
		SQL = delete_check(SQL, start);
	else if (tp == "create")
		SQL = create_check(SQL, start);
	else if (tp == "drop")
		SQL = drop_check(SQL, start);
	
	else if (tp == "use")
		SQL = use_check(SQL, start);
	else if (tp == "quit")
		SQL = quit_check(SQL, start);
	else if (tp == "help")
		SQL = "80";
	//��Ϊ�Ƿ�����
	else
	{
		cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//�������������ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤select ����Ƿ���Ч
string select_check(string SQL, int start)
{
	int end;
	string temp;
	string attr;
	string table;
	string condition;
	end = SQL.find("from", start);
	if (end == -1)	//û��from
	{
		cout << "syntax error: lack of 'from'" << endl;
		SQL = "99";
	}
	else
	{
		temp = SQL.substr(start, end - start);	//ѡ������
		temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//ȥ�ո�
		start = end + 4;
		if (temp.empty())	//�Ƿ�������
		{
			cout << "syntax error: please specify the attributes" << endl;
			SQL = "99";
		}
		else
		{
			attr = temp;
			start = SQL.find_first_of(allchar, start);
			end = SQL.find_first_not_of(allchar, start);
			if (end == -1)
			{
				cout << "syntax error: please specify the Table" << endl;
				SQL = "99";
			}
			else
			{
				table = SQL.substr(start, end - start);	//����
														//cout << attr << endl << start << endl << end << endl << table << endl;

				if (table == "where")
				{
					cout << "syntax error: please specify the Table" << endl;
					SQL = "99";
					return SQL;
				}
				start = end + 1;
				end = SQL.find_first_not_of(allchar, start);
				temp = SQL.substr(start, end - start);	//ȡwhere

				if (temp != "where")	//�Ƿ���where
				{
					//��where��
					if (end < SQL.length() - 1)
					{
						//ȱ�ٴ�������'where'ƴд����
						cout << "syntax error: please specify the Table or may be something wrong with 'where' " << endl;
						SQL = "99";
					}
					else
					{
						//��where����ȷ��䣻																	//	TO-DO
						//cout << "fuck" << endl;
						SQL = "20" + table + ' ' + attr;
					}
				}
				else
				{
					//��where��
					//cout << temp << endl;
					if (table.empty())
					{
						//ȱ�ٴ����
						cout << "syntax error: please specify the tables" << endl;
						SQL = "99";
					}
					else
					{
						//�д����
						start = end + 1;
						temp = SQL.substr(start, SQL.length() - start - 1);

						size_t index = 0;
						while (true) {	//�滻" and "Ϊ'&'
							index = temp.find(" and ", index);
							//cout << temp << endl << index << endl;
							if (index == string::npos) break;
							temp.replace(index, 5, "&");
							index += 1;
						}
						index = 0;
						while (true) {	//�滻" or "Ϊ'|'
							index = temp.find(" or ", index);
							//cout << temp << endl << index << endl;
							if (index == string::npos) break;
							temp.replace(index, 5, "|");
							index += 1;
						}

						//cout << start << endl << end << endl << temp << endl;
						temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//ȥ�ո�
						if (temp.empty())	//�Ƿ���where����
						{
							//ȱ��where����
							cout << "syntax error: please specify the condition or remove 'where' " << endl;
							SQL = "99";
						}
						else
						{
							//��where����
							condition = temp;
							SQL = "21" + table + ' ' + attr + ' ' + condition;
						}
					}
				}
			}
		}
	}
	//cout << attr << ' ' << table << ' ' << condition << endl;
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//�����������ļ����ÿһ��
string get_part(string temp, string sql, string kind)
{
	return sql;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//��֤execfile����Ƿ���Ч
string execfile_check(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	end = SQL.find_last_of(allchar) + 1;
	temp = SQL.substr(start, end - start);
	end = SQL.find_first_not_of(' ', end);
	start = end + 1;

	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for execfile statement!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid file name!" << endl;
			SQL = "99";
		}
		//����create database�����ڲ���ʽ
		else
			SQL = "60" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤insert����Ƿ���Ч
string insert_check(string SQL, int start)  //insert into ���� values ( ֵ1 , ֵ2 , �� , ֵn ) ;
{
	string temp;
	string sql = "";
	int end;

	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);  //temp = "into"

	start = end + 1;
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: can't find the keyword 'into' " << endl;
		SQL = "99";
	}
	else if (temp != "into")
	{
		cout << "syntax error: might be 'into' " << endl;
		SQL = "99";
	}
	else
	{
		//��ȡ����������
		start = SQL.find_first_not_of(' ', start);
		//index = start;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);  //temp = table��
		sql += temp;   //sql = table��
		start = end + 1;

		//���ޣ���ӡ������Ϣ
		if (temp.empty())
		{
			cout << "syntax error: can't find the target Table" << endl;
			SQL = "99";
		}
		else if (temp == "values" || temp == "value" || temp == "into")
		{
			cout << "syntax error: can't find the target Table" << endl;
			SQL = "99";
		}
		else
		{

			//��ȡ���ĸ�����
			start = SQL.find_first_not_of(' ', start);
			//index = start;
			end = SQL.find_first_not_of(allchar, start);
			temp = SQL.substr(start, end - start);  //temp = value
			start = end;                    //start = (

											//���ޣ���ӡ������Ϣ
			if (temp.empty())
			{
				cout << "syntax error: can't find keyword 'values' or 'value' " << endl;
				SQL = "99";
			}
			else if (temp != "values" && temp != "value")
			{
				cout << "syntax error: maybe 'value' or 'values' " << endl;
				SQL = "99";
			}
			else
			{
				//cout << SQL;
				SQL = insert_into_values(SQL, start, sql);
			}
		}
	}

	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤insert into values����Ƿ���Ч
string insert_into_values(string SQL, int start, string sql)
{
	string temp;                      //insert into ���� values ( ֵ1 , ֵ2 , �� , ֵn ) ;
	string T;
	int end;
	//��ȡ(
	start = SQL.find_first_not_of(' ', start);
	end = SQL.find_first_not_of('(', start);
	temp = SQL.substr(start, end - start);  //temp = (
	start = end;
	//���ޣ���ӡ����
	if (temp.empty())
	{
		cout << "syntax error: lack of '('" << endl;
		SQL = "99";
	}
	else if (temp != "(")
	{
		cout << "syntax error: multiple '(' have been found" << endl; //������ȱʧ�����
		SQL = "99";
	}
	else
	{
		end = SQL.rfind(')', SQL.length());	//���һ��������
		if (end == -1)
		{
			cout << "syntax error: lack of ')'" << endl;
			SQL = "99";
		}
		else
		{
			temp = SQL.substr(start, end - start);  //temp = ( ֵ1 , ֵ2 , �� , ֵn )
			temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//ȥ�ո�
			start = end + 1;
			//���ޣ���ӡ����
			if (temp.empty())
			{
				cout << "syntax error: syntax error for insert into statement!" << endl;
				SQL = "99";
			}
			else
			{
				//replace(temp.begin(), temp.end(), ' ', ''); // replace allchar 'x' to 'y'
				SQL = "30" + sql + ' ' + temp;
				//cout << SQL;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤delete����Ƿ���Ч
string delete_check(string SQL, int start)
{
	int end;
	string temp;
	string table;
	end = SQL.find("from", start);
	if (end == -1)
	{
		//û��from
		cout << "syntax error: lack of 'from' " << endl;
		SQL = "99";
	}
	else
	{
		start = end + 5;	//ָ�����
		end = SQL.find_first_not_of(allchar, start);
		temp = SQL.substr(start, end - start);	//ȡ������������ȱʧʱȡ��where;
		if (temp.empty() || temp == "where")
		{
			//����Ϊ��
			cout << "syntax error: please specify the target Table " << endl;
			SQL = "99";
		}
		else
		{
			//�б���
			table = temp;
			//cout << table << endl;
			start = end + 1;
			end = SQL.find_first_not_of(allchar, start);
			temp = SQL.substr(start, end - start);	//ȡ������֮��Ĵ���������';'������'where'���������ʽ
													//cout << start << endl << end << endl << temp << endl;
			if (temp.empty() || temp == ";")
			{
				//��where��ȷ����;																			//TO-DO
				SQL = "40" + table;
			}
			else if (temp == "where")
			{
				//��where��ȷ����;
				start = end;
				SQL = delete_from_where(SQL, start, table);
			}
			else
			{
				//where����
				cout << "syntax error: lack of 'where' " << endl;
				SQL = "99";
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤ delete from where ����Ƿ���Ч
string delete_from_where(string SQL, int start, string sql)
{
	int end;
	string temp;

	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);

	size_t index = 0;
	while (true) {	//�滻" and "Ϊ','

		index = temp.find(" and ", index);
		//cout << temp << endl << index << endl;
		if (index == string::npos) break;
		temp.replace(index, 5, "&");
		index += 1;
	}
	index = 0;
	while (true) {	//�滻" or "Ϊ'|'
		index = temp.find(" or ", index);
		//cout << temp << endl << index << endl;
		if (index == string::npos) break;
		temp.replace(index, 5, "|");
		index += 1;
	}

	temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//ȥ�ո�

	if (temp.empty())
	{
		cout << "syntax error: please specify condition " << endl;
		SQL = "99";
	}
	else
	{
		//where ��ȷ����;
		SQL = "41" + sql + ' ' + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//�����ʽת��Ϊ�ڲ���ʽ
string get_expression(string temp, string sql)
{
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ���ʽ���ÿ�����ʽ
string get_each(string T, string sql, string condition)
{
	return sql;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create����Ƿ���Ч
string create_check(string SQL, int start)
{
	string tp;
	int end;
	string tp2;
	int i,end2;
	int start2;
	//��ȡ�ڶ�������
	end = SQL.find_first_of(' ', start);
	tp = SQL.substr(start, end - start);
	start = end + 1;  
	//û�е��������ʣ���ӡ������Ϣ
	if (start == 0 || tp.empty())
	{   
		printf("syntax error: No table/database name!\n");
		SQL = "99";
	}
	//���ڶ�������Ϊtable
	else if (tp == "table")
		SQL = create_table(SQL, start);
	//���ڶ�������Ϊdatabase
	else if (tp == "database"){
		start2 = start;
	//��һ����֤create database����Ƿ���Ч
		end2 = SQL.find_first_of(';', start2);
		tp2 = SQL.substr(start2, end2 - start2);  //���ݿ���
		i = 1;
		while (i < 100) {
			i++;
		}
		start2 = end2 + 1;
		//�������֣���ӡ������Ϣ
		if (start2 == 0 || tp2.empty())
		{
			printf("syntax error: No database name\n");
			SQL = "99";
		}
		else
		{
			//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
			if (SQL.at(end2) != ';' || end2 != SQL.length() - 1)
			{
				cout << "error:" << tp << "---is not a valid database name!" << endl;
				SQL = "99";
			}
			//����create database�����ڲ���ʽ
			else
				SQL = "00" + tp2;
		}
		return SQL;
	}

	//���ڶ�������Ϊindex
	else if (tp == "index")
		SQL = create_index(SQL, start);
	//��Ϊ������Ϣ
	else
	{   
		cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//����create�����ڲ���ʽ
	return SQL;
}



//��һ����֤create table����Ƿ���Ч
string create_table(string SQL, int start)
{
	string tp, sql, T;
	int end;
	//��ȡ����
	end = SQL.find_first_not_of(allchar, start);
	tp = SQL.substr(start, end - start);
	if ((SQL.find('(', end)) == -1)
	{   
		printf("error: missing in the statement!\n");
		SQL = "99";
		return SQL;
	}
	start = SQL.find_first_of('(', end) + 1;
	//��ӡ������Ϣ
	if (start == 0 || tp.empty())
	{
		printf("error: failed in create table statement!\n");
		SQL = "99";
		return SQL;
	}
	//����������
	else
	{
		sql = tp + ",";
		start = SQL.find_first_of(allchar, start);
		//������ȡÿ������
		end = SQL.find(',', start);
		while ((end != -1)
		{
			tp = SQL.substr(start, end - start);  //��һ������
			start = end + 1;
			//����Ϊ�գ���ӡ������Ϣ
			if (start == 0 || tp.empty())getattri
			{
				printf("error: failed in create table statement!\n");
				SQL = "99";
				return SQL;
			}
			//��������
			else
			{
				sql = getattri(tp, sql);
				if (sql == "99")
					return sql;
			}
			while (SQL.at(start) == ' ' || SQL.at(start) == '\n')
				start++;
		}
		//����������
		end = SQL.find_last_of(')');
		tp = SQL.substr(start, end - start);
		if (tp.empty() || tp.find_last_of(')') == string::npos)
		{
			printf("error: failed in create table statement!\n");
			SQL = "99";
			return SQL;
		}
		//�洢
		else
		{
			end = SQL.find_first_of(' ', start);
			T = SQL.substr(start, end - start);
			start = end + 1;
			
			if (T != "primary")   //����������
			{
				sql = getattri(tp, sql);
				if (sql == "99")
				{
					SQL = "99";
					return SQL;
				}
				else
					SQL = "01" + sql;
			}
			//��������
			else
			{
				//�ж��Ƿ��йؼ���key
				while (SQL.at(start) == ' ')
					start++;
				end = SQL.find_first_not_of(allchar, start);
				T = SQL.substr(start, end - start);
				start = end;
				if (T.empty())
				{
					cout << "\t missing key word key!" << endl;
					SQL = "99";
					return SQL;
				}
				//������֤
				else if (T == "key")
				{
					//��ȡ����������
					start = SQL.find_first_of('(', start);
					start = SQL.find_first_of(allchar, start);
					end = SQL.find_first_not_of(allchar, start);
					T = SQL.substr(start, end - start);
					if (T.empty())
					{
						printf("error:  missing primary key!\n");
						SQL = "99";
						return SQL;
					}
					//��Ϊ�Ƿ���Ϣ�����
					else if (T.find(' ') != -1 || (sql.find(T) == -1))
					{
						cout << "error : " << T << "---is not a valid primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//����
					else
					{
						sql += T + " #,";
						SQL = "01" + sql;
					}
				}
				//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
				else
				{
					cout << "error : " << T << "---is not a valid key word!" << endl;
					SQL = "99";
					return SQL;
				}
			}
		}
	}
	return SQL;
}

//�������
string getattri(string tp, string sql)
{
	int start = 0;
	int i;
	string name, type, C, us;
	int end, index;
	tp += " ";
	//���������
	end = tp.find_first_of(' ', start);
	name = tp.substr(start, end - start);
	for (i = 0;i < 10;i++);
	sql = sql + name + ' ';
	start = end + 1;
	//�����������
	while (tp.at(start) == ' ') {
		i++;
		start++;
	}
		end = tp.find(' ', start);
	type = tp.substr(start, end - start);
	start = end;
	i = tp.length() - 1;
	//������ȱ����ӡ������Ϣ
	if (name.empty() || type.empty())
	{
		printf("error: failed in create table statement!\n");
		sql = "99";
		return sql;
	}
	else if (type == "float")
		sql += "-";
	else if (type == "int")
		sql += "+";
	else
	{
		index = type.find('(');
		C = type.substr(0, index);
	
		if (C.empty())
		{
			cout << "error: " << type << "---is not a valid data type definition!" << endl;
			sql = "99";
			return sql;
		}
		//��CΪchar
		for (i = 0;i < 10;i++);
		index++;
		else if (C == "char")
		{
			C = type.substr(index, type.length() - index - 1);
			if (C.empty())
			{   
				printf("error: the length of the data type char has not been specified!!!\n");
				sql = "99";
				i++;
				return sql;
			}
			else {
		
				sql = sql + C;
					
			}
		}
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout << "error: " << C << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//check������Ϣ
	while (start < tp.length() - 1 && tp.at(start) == ' ')
		start++;
	i= tp.length() - 1;
	if (start < i)
	{
		//��Ϊunique����
		end = tp.find_first_of(' ', start);
		us = tp.substr(start, end - start);
		if (us == "unique")
		{
			sql += " 1,";
		}
		//��Ϊ�Ƿ���Ϣ
		else
		{
			cout << "error: " << us << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//���޸�����Ϣ
	else
		sql += " 0,";
	return sql;
}


//��֤create index����Ƿ���Ч
string create_index(string SQL, int start)
{
	string tp, sql;
	int end,i;
	//��ȡ����������
	while (SQL.at(start) == ' ')
		start++;
	end = SQL.find_first_of(' ', start);
	tp = SQL.substr(start, end - start);
	start = end + 1;
	if (start == 0 || tp.empty())
	{
		printf("syntax error : No index name!\n");
		SQL = "99";
	}
	else
	{
		sql = tp;
		end = 0;
		//��ȡ���ĸ�����
		while (SQL.at(start) == ' ')
			start++;
		for (i = 1;i <= 100;i++) end++;
		end = SQL.find_first_of(' ', start);
		tp = SQL.substr(start, end - start);
		start = end + 1;
		
		//���޵��ĸ����ʣ���ӡ������Ϣ
		if (start == 0 || tp.empty())
		{  
			printf("syntax error: incomplete statement!\n");
			SQL = "99";
		}
		//������֤
		else if (tp == "on"){
			//���֮ǰ���﷨������ȷ��
			string tp23;
			int end=0,int length;
			for (i = 1;i <= 100;i++)end++;
			//��ȡ����
			while (SQL.at(start) == ' ')
				start++;
			end = SQL.find_first_of('(', start);
			tp23 = SQL.substr(start, end - start);
			start = end + 1;
			if (start == 0 || tp23.empty())
			{
				printf("syntax error: incomplete statement!!\n");
				SQL = "99";
	        } 
			else
			{
				//�����Ƿ�Ϊ��Ч�ļ���
				length = tp23.length() - 1;
				while (tp23.at(length) == ' ')
					length--;
				tp23 = tp23.substr(0, length + 1);
				//��Ч                            
				if (tp23.find(' ') == -1) // ��һ�±��Ƿ����
				{
					sql += " " + tp23;
					//��ȡ������
					while (SQL.at(start) == ' ')
						start++;
					end = SQL.find(')', start);
					tp23 = SQL.substr(start, end - start);
					start = end + 1;
					//���������
					if (start == 0 || tp23.empty())
					{
						printf("syntax error: incomplete statement!!\n");
						SQL = "99";
					}
					else
					{
						//�����Ƿ�Ϊ��Ч��
						length = tp23.length() - 1;
						while (tp23.at(length) == ' ')
							length--;
						tp23 = tp23.substr(0, length + 1);
						//��Ч
						if (tp23.find(' ') == -1)	// ��������Ƿ����
						{
							sql += " " + tp23;
							while (SQL.at(start) == ' ')
								start++;
							if (SQL.at(start) != ';' || start != SQL.length() - 1)
							{
								printf("syntax error: failed in create index on statement!\n");
								SQL = "99";
							}
							//����create index�����ڲ���ʽ
							else
								SQL = "02" + sql;
						}
						else
						{
							cout << "error:" << " " << tp23 << "---is not a valid attribute name!" << endl;
							SQL = "99";
						}
					}
				}
				else
				{
					cout << "error:" << " " << tp23 << "---is not a valid table name!" << endl;
					SQL = "99";
				}
			}
		}
		//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
		else
		{
			cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
			SQL = "99";
		}
	}
	return SQL;
}

//��֤drop����Ƿ���Ч
string drop_check(string SQL, int start)
{

	string tp;
	int end,i;
	end = SQL.find_first_of(' ', start);
	tp = SQL.substr(start, end - start);
	start = end + 1;  
	//����drop����
	if (start == 0 || tp.empty())
	{	for (i = 1;i <= 10;i++);
		printf("syntax error: incomplete input!!\n");
		SQL = "99";
	}
	//��Ϊtable
	else if (tp == "table")
		SQL = drop_table(SQL, start);
	//��Ϊdatabase
	else if (tp == "database")
		SQL = drop_database(SQL, start);
	//��Ϊindex,������֤
	else if (tp == "index")
		SQL = drop_index(SQL, start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{	cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//����drop�����ڲ���ʽ
	return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop database����Ƿ���Ч
string drop_database(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ����������
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //���ݿ���
	start = end + 1;  //start�ӵ��ĸ��ʿ�ʼ
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//����drop database�����ڲ���ʽ
		else
			SQL = "10" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop table����Ƿ���Ч
string drop_table(string SQL, int start)  //drop table ���� ;
{
	string temp;
	int end;
	//��ȡ����������
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //���ݿ���
	start = end + 1;  //start�ӵ��ĸ��ʿ�ʼ
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid table name!" << endl;
			SQL = "99";
		}
		//����drop table�����ڲ���ʽ
		else
			SQL = "11" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop index����Ƿ���Ч
string drop_index(string SQL, int start)  //drop index ������ ;
{
	string temp;
	int end;
	//��ȡ����������
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //���ݿ���
	start = end + 1;  //start�ӵ��ĸ��ʿ�ʼ
	//���ޣ���ӡ������Ϣ

	if (start == 0 || temp.empty())
	{
		cout << "error: index name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid index name!" << endl;
			SQL = "99";
		}
		//����drop index�����ڲ���ʽ
		else
			SQL = "12" + temp;
	}
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//��֤use����Ƿ���Ч
string use_check(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	end = SQL.find_first_not_of(allchar, start);
	temp = SQL.substr(start, end - start);
	end = SQL.find_first_not_of(' ', end);
	start = end + 1;

	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for use statement!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//����create database�����ڲ���ʽ
		else
			SQL = "50" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤quit����Ƿ���Ч
string quit_check(string SQL, int start)
{
	SQL = "70";
	return SQL;
}
