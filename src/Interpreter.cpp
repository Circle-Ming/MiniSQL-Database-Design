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

	switch (this->type)			//根据语句种类来判断 
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


		start = conditionstart + 1;      //分析条件 
		for (;;) {
			if (start != 0) break;	// 条件取完
			end = SQL.find_first_of("&|", start);
			temp = SQL.substr(start, end - start);	// 取条件
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
		start = end + 1;	// 获取表名 
		content = SQL.substr(start);     //获取内容 
		break;

	case MYDELETE:
		tableName = SQL.substr(start);   //读取表名 
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
			conditions.push_back(genCondition(temp));		//将生成存入
			start = end + 1;
		}
		break;

	case CREATE_DATABASE:break;

	case CREATE_TABLE:
		end = SQL.find_first_of(',', start);
		te = start - end;
		tableName = SQL.substr(start, end - start);
		start = end + 1;	// 读取表名

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
			if (second == -1) {				// 有primarykey
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
//获取输入语句，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string SQL)
{
	string tp;
	int start = 0, end;
	if (SQL[0] == '#') {
		cout << "comment ignored\n" << endl;
		SQL = "98";
		return SQL;
	}
	//获取输入的第一个单词
	start = SQL.find_first_not_of(' ', 0);
	end = SQL.find_first_not_of(allchar, start);
	tp = SQL.substr(start, end - start);//第一个词
	//获取第二个单词
	start = end + 1;
	//分别调用对应函数检测有效性
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
	//若为非法输入
	else
	{
		cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回输入语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证select 语句是否有效
string select_check(string SQL, int start)
{
	int end;
	string temp;
	string attr;
	string table;
	string condition;
	end = SQL.find("from", start);
	if (end == -1)	//没有from
	{
		cout << "syntax error: lack of 'from'" << endl;
		SQL = "99";
	}
	else
	{
		temp = SQL.substr(start, end - start);	//选择属性
		temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//去空格
		start = end + 4;
		if (temp.empty())	//是否有属性
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
				table = SQL.substr(start, end - start);	//表名
														//cout << attr << endl << start << endl << end << endl << table << endl;

				if (table == "where")
				{
					cout << "syntax error: please specify the Table" << endl;
					SQL = "99";
					return SQL;
				}
				start = end + 1;
				end = SQL.find_first_not_of(allchar, start);
				temp = SQL.substr(start, end - start);	//取where

				if (temp != "where")	//是否有where
				{
					//无where；
					if (end < SQL.length() - 1)
					{
						//缺少待查表或者'where'拼写有误
						cout << "syntax error: please specify the Table or may be something wrong with 'where' " << endl;
						SQL = "99";
					}
					else
					{
						//无where的正确语句；																	//	TO-DO
						//cout << "fuck" << endl;
						SQL = "20" + table + ' ' + attr;
					}
				}
				else
				{
					//有where；
					//cout << temp << endl;
					if (table.empty())
					{
						//缺少待查表
						cout << "syntax error: please specify the tables" << endl;
						SQL = "99";
					}
					else
					{
						//有待查表
						start = end + 1;
						temp = SQL.substr(start, SQL.length() - start - 1);

						size_t index = 0;
						while (true) {	//替换" and "为'&'
							index = temp.find(" and ", index);
							//cout << temp << endl << index << endl;
							if (index == string::npos) break;
							temp.replace(index, 5, "&");
							index += 1;
						}
						index = 0;
						while (true) {	//替换" or "为'|'
							index = temp.find(" or ", index);
							//cout << temp << endl << index << endl;
							if (index == string::npos) break;
							temp.replace(index, 5, "|");
							index += 1;
						}

						//cout << start << endl << end << endl << temp << endl;
						temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//去空格
						if (temp.empty())	//是否有where条件
						{
							//缺少where条件
							cout << "syntax error: please specify the condition or remove 'where' " << endl;
							SQL = "99";
						}
						else
						{
							//有where条件
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
//获得属性组或文件组的每一项
string get_part(string temp, string sql, string kind)
{
	return sql;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//验证execfile语句是否有效
string execfile_check(string SQL, int start)
{
	string temp;
	int end;
	//获取第二个单词
	end = SQL.find_last_of(allchar) + 1;
	temp = SQL.substr(start, end - start);
	end = SQL.find_first_not_of(' ', end);
	start = end + 1;

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for execfile statement!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid file name!" << endl;
			SQL = "99";
		}
		//返回create database语句的内部形式
		else
			SQL = "60" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert语句是否有效
string insert_check(string SQL, int start)  //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
{
	string temp;
	string sql = "";
	int end;

	end = SQL.find(' ', start);
	temp = SQL.substr(start, end - start);  //temp = "into"

	start = end + 1;
	//若无，打印出错信息
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
		//获取第三个单词
		start = SQL.find_first_not_of(' ', start);
		//index = start;
		end = SQL.find(' ', start);
		temp = SQL.substr(start, end - start);  //temp = table名
		sql += temp;   //sql = table名
		start = end + 1;

		//若无，打印出错信息
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

			//获取第四个单词
			start = SQL.find_first_not_of(' ', start);
			//index = start;
			end = SQL.find_first_not_of(allchar, start);
			temp = SQL.substr(start, end - start);  //temp = value
			start = end;                    //start = (

											//若无，打印出错信息
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
//验证insert into values语句是否有效
string insert_into_values(string SQL, int start, string sql)
{
	string temp;                      //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
	string T;
	int end;
	//获取(
	start = SQL.find_first_not_of(' ', start);
	end = SQL.find_first_not_of('(', start);
	temp = SQL.substr(start, end - start);  //temp = (
	start = end;
	//若无，打印出错
	if (temp.empty())
	{
		cout << "syntax error: lack of '('" << endl;
		SQL = "99";
	}
	else if (temp != "(")
	{
		cout << "syntax error: multiple '(' have been found" << endl; //左括号缺失或过多
		SQL = "99";
	}
	else
	{
		end = SQL.rfind(')', SQL.length());	//最后一个右括号
		if (end == -1)
		{
			cout << "syntax error: lack of ')'" << endl;
			SQL = "99";
		}
		else
		{
			temp = SQL.substr(start, end - start);  //temp = ( 值1 , 值2 , … , 值n )
			temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//去空格
			start = end + 1;
			//若无，打印出错
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
//验证delete语句是否有效
string delete_check(string SQL, int start)
{
	int end;
	string temp;
	string table;
	end = SQL.find("from", start);
	if (end == -1)
	{
		//没有from
		cout << "syntax error: lack of 'from' " << endl;
		SQL = "99";
	}
	else
	{
		start = end + 5;	//指向表名
		end = SQL.find_first_not_of(allchar, start);
		temp = SQL.substr(start, end - start);	//取表名，当表名缺失时取到where;
		if (temp.empty() || temp == "where")
		{
			//表名为空
			cout << "syntax error: please specify the target Table " << endl;
			SQL = "99";
		}
		else
		{
			//有表名
			table = temp;
			//cout << table << endl;
			start = end + 1;
			end = SQL.find_first_not_of(allchar, start);
			temp = SQL.substr(start, end - start);	//取到表名之后的串，可能是';'可能是'where'及其错误形式
													//cout << start << endl << end << endl << temp << endl;
			if (temp.empty() || temp == ";")
			{
				//无where正确出口;																			//TO-DO
				SQL = "40" + table;
			}
			else if (temp == "where")
			{
				//有where正确出口;
				start = end;
				SQL = delete_from_where(SQL, start, table);
			}
			else
			{
				//where错误
				cout << "syntax error: lack of 'where' " << endl;
				SQL = "99";
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证 delete from where 语句是否有效
string delete_from_where(string SQL, int start, string sql)
{
	int end;
	string temp;

	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);

	size_t index = 0;
	while (true) {	//替换" and "为','

		index = temp.find(" and ", index);
		//cout << temp << endl << index << endl;
		if (index == string::npos) break;
		temp.replace(index, 5, "&");
		index += 1;
	}
	index = 0;
	while (true) {	//替换" or "为'|'
		index = temp.find(" or ", index);
		//cout << temp << endl << index << endl;
		if (index == string::npos) break;
		temp.replace(index, 5, "|");
		index += 1;
	}

	temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());	//去空格

	if (temp.empty())
	{
		cout << "syntax error: please specify condition " << endl;
		SQL = "99";
	}
	else
	{
		//where 正确出口;
		SQL = "41" + sql + ' ' + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//将表达式转化为内部形式
string get_expression(string temp, string sql)
{
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取表达式组的每个表达式
string get_each(string T, string sql, string condition)
{
	return sql;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string create_check(string SQL, int start)
{
	string tp;
	int end;
	string tp2;
	int i,end2;
	int start2;
	//获取第二个单词
	end = SQL.find_first_of(' ', start);
	tp = SQL.substr(start, end - start);
	start = end + 1;  
	//没有第三个单词，打印出错信息
	if (start == 0 || tp.empty())
	{   
		printf("syntax error: No table/database name!\n");
		SQL = "99";
	}
	//若第二个单词为table
	else if (tp == "table")
		SQL = create_table(SQL, start);
	//若第二个单词为database
	else if (tp == "database"){
		start2 = start;
	//进一步验证create database语句是否有效
		end2 = SQL.find_first_of(';', start2);
		tp2 = SQL.substr(start2, end2 - start2);  //数据库名
		i = 1;
		while (i < 100) {
			i++;
		}
		start2 = end2 + 1;
		//若无名字，打印出错信息
		if (start2 == 0 || tp2.empty())
		{
			printf("syntax error: No database name\n");
			SQL = "99";
		}
		else
		{
			//若为非法信息，打印出错信息
			if (SQL.at(end2) != ';' || end2 != SQL.length() - 1)
			{
				cout << "error:" << tp << "---is not a valid database name!" << endl;
				SQL = "99";
			}
			//返回create database语句的内部形式
			else
				SQL = "00" + tp2;
		}
		return SQL;
	}

	//若第二个单词为index
	else if (tp == "index")
		SQL = create_index(SQL, start);
	//若为错误信息
	else
	{   
		cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回create语句的内部形式
	return SQL;
}



//进一步验证create table语句是否有效
string create_table(string SQL, int start)
{
	string tp, sql, T;
	int end;
	//获取表名
	end = SQL.find_first_not_of(allchar, start);
	tp = SQL.substr(start, end - start);
	if ((SQL.find('(', end)) == -1)
	{   
		printf("error: missing in the statement!\n");
		SQL = "99";
		return SQL;
	}
	start = SQL.find_first_of('(', end) + 1;
	//打印出错信息
	if (start == 0 || tp.empty())
	{
		printf("error: failed in create table statement!\n");
		SQL = "99";
		return SQL;
	}
	//正常的输入
	else
	{
		sql = tp + ",";
		start = SQL.find_first_of(allchar, start);
		//挨个获取每个属性
		end = SQL.find(',', start);
		while ((end != -1)
		{
			tp = SQL.substr(start, end - start);  //第一个属性
			start = end + 1;
			//属性为空，打印出错信息
			if (start == 0 || tp.empty())getattri
			{
				printf("error: failed in create table statement!\n");
				SQL = "99";
				return SQL;
			}
			//属性正常
			else
			{
				sql = getattri(tp, sql);
				if (sql == "99")
					return sql;
			}
			while (SQL.at(start) == ' ' || SQL.at(start) == '\n')
				start++;
		}
		//获得最后属性
		end = SQL.find_last_of(')');
		tp = SQL.substr(start, end - start);
		if (tp.empty() || tp.find_last_of(')') == string::npos)
		{
			printf("error: failed in create table statement!\n");
			SQL = "99";
			return SQL;
		}
		//存储
		else
		{
			end = SQL.find_first_of(' ', start);
			T = SQL.substr(start, end - start);
			start = end + 1;
			
			if (T != "primary")   //非主键定义
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
			//主键定义
			else
			{
				//判断是否有关键字key
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
				//继续验证
				else if (T == "key")
				{
					//获取主键属性名
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
					//若为非法信息则出错！
					else if (T.find(' ') != -1 || (sql.find(T) == -1))
					{
						cout << "error : " << T << "---is not a valid primary key attribute name!" << endl;
						SQL = "99";
						return SQL;
					}
					//保存
					else
					{
						sql += T + " #,";
						SQL = "01" + sql;
					}
				}
				//若为非法信息，打印出错信息
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

//获得属性
string getattri(string tp, string sql)
{
	int start = 0;
	int i;
	string name, type, C, us;
	int end, index;
	tp += " ";
	//获得属性名
	end = tp.find_first_of(' ', start);
	name = tp.substr(start, end - start);
	for (i = 0;i < 10;i++);
	sql = sql + name + ' ';
	start = end + 1;
	//获得数据类型
	while (tp.at(start) == ' ') {
		i++;
		start++;
	}
		end = tp.find(' ', start);
	type = tp.substr(start, end - start);
	start = end;
	i = tp.length() - 1;
	//若语句残缺，打印出错信息
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
		//若C为char
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
		//若为非法信息，打印出错信息
		else
		{
			cout << "error: " << C << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//check附加信息
	while (start < tp.length() - 1 && tp.at(start) == ' ')
		start++;
	i= tp.length() - 1;
	if (start < i)
	{
		//若为unique定义
		end = tp.find_first_of(' ', start);
		us = tp.substr(start, end - start);
		if (us == "unique")
		{
			sql += " 1,";
		}
		//若为非法信息
		else
		{
			cout << "error: " << us << "---is not a valid key word!" << endl;
			sql = "99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql += " 0,";
	return sql;
}


//验证create index语句是否有效
string create_index(string SQL, int start)
{
	string tp, sql;
	int end,i;
	//获取第三个单词
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
		//获取第四个单词
		while (SQL.at(start) == ' ')
			start++;
		for (i = 1;i <= 100;i++) end++;
		end = SQL.find_first_of(' ', start);
		tp = SQL.substr(start, end - start);
		start = end + 1;
		
		//若无第四个单词，打印出错信息
		if (start == 0 || tp.empty())
		{  
			printf("syntax error: incomplete statement!\n");
			SQL = "99";
		}
		//继续验证
		else if (tp == "on"){
			//如果之前的语法都是正确的
			string tp23;
			int end=0,int length;
			for (i = 1;i <= 100;i++)end++;
			//获取表名
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
				//检验是否为有效文件名
				length = tp23.length() - 1;
				while (tp23.at(length) == ' ')
					length--;
				tp23 = tp23.substr(0, length + 1);
				//有效                            
				if (tp23.find(' ') == -1) // 查一下表是否存在
				{
					sql += " " + tp23;
					//获取属性名
					while (SQL.at(start) == ' ')
						start++;
					end = SQL.find(')', start);
					tp23 = SQL.substr(start, end - start);
					start = end + 1;
					//若无则出错
					if (start == 0 || tp23.empty())
					{
						printf("syntax error: incomplete statement!!\n");
						SQL = "99";
					}
					else
					{
						//检验是否为有效名
						length = tp23.length() - 1;
						while (tp23.at(length) == ' ')
							length--;
						tp23 = tp23.substr(0, length + 1);
						//有效
						if (tp23.find(' ') == -1)	// 检测属性是否存在
						{
							sql += " " + tp23;
							while (SQL.at(start) == ' ')
								start++;
							if (SQL.at(start) != ';' || start != SQL.length() - 1)
							{
								printf("syntax error: failed in create index on statement!\n");
								SQL = "99";
							}
							//返回create index语句的内部形式
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
		//若为非法信息，打印非法信息
		else
		{
			cout << "syntax error:" << " " << tp << "---is not a valid key word!" << endl;
			SQL = "99";
		}
	}
	return SQL;
}

//验证drop语句是否有效
string drop_check(string SQL, int start)
{

	string tp;
	int end,i;
	end = SQL.find_first_of(' ', start);
	tp = SQL.substr(start, end - start);
	start = end + 1;  
	//若无drop对象
	if (start == 0 || tp.empty())
	{	for (i = 1;i <= 10;i++);
		printf("syntax error: incomplete input!!\n");
		SQL = "99";
	}
	//若为table
	else if (tp == "table")
		SQL = drop_table(SQL, start);
	//若为database
	else if (tp == "database")
		SQL = drop_database(SQL, start);
	//若为index,继续验证
	else if (tp == "index")
		SQL = drop_index(SQL, start);
	//若为错误信息，打印出错信息
	else
	{	cout << "syntax error:" << " " << temp << "---is not a valid key word!" << endl;
		SQL = "99";
	}
	//返回drop语句的内部形式
	return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string drop_database(string SQL, int start)
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回drop database语句的内部形式
		else
			SQL = "10" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string drop_table(string SQL, int start)  //drop table 表名 ;
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: table name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid table name!" << endl;
			SQL = "99";
		}
		//返回drop table语句的内部形式
		else
			SQL = "11" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string drop_index(string SQL, int start)  //drop index 索引名 ;
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始
	//若无，打印出错信息

	if (start == 0 || temp.empty())
	{
		cout << "error: index name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid index name!" << endl;
			SQL = "99";
		}
		//返回drop index语句的内部形式
		else
			SQL = "12" + temp;
	}
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证use语句是否有效
string use_check(string SQL, int start)
{
	string temp;
	int end;
	//获取第二个单词
	end = SQL.find_first_not_of(allchar, start);
	temp = SQL.substr(start, end - start);
	end = SQL.find_first_not_of(' ', end);
	start = end + 1;

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "syntax error: syntax error for use statement!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回create database语句的内部形式
		else
			SQL = "50" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证quit语句是否有效
string quit_check(string SQL, int start)
{
	SQL = "70";
	return SQL;
}
