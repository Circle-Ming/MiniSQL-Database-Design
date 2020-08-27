#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <string.h>
#include "MiniSQL.h"

//以下是判断statement的辅助函数
inline TYPE getType(string s);

inline bool isUnique(string s);

void SQLstatement::outputinfo();

SQLstatement::SQLstatement(string SQL);

STATEMENT_TYPE SQLstatement::identify(string type_code);

Condition SQLstatement::genCondition(string exp);

RELATION_TYPE SQLstatement::getRelationType(string oper);

SQLstatement::SQLstatement(STATEMENT_TYPE type, string tableName);

SQLstatement::~SQLstatement();
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string SQL);

//验证select 语句是否有效
string select_check(string SQL, int start);

//获得属性组或文件组的每一项
string get_part(string temp, string sql, string kind);

//验证execfile语句是否有效
string execfile_check(string SQL, int start);

//验证insert 语句是否有效
string insert_check(string SQL, int start);

//验证insert into values语句是否有效
string insert_into_values(string SQL, int start, string sql);

//验证delete语句是否有效
string delete_check(string SQL, int start);

//验证 delete from where 语句是否有效
string delete_from_where(string SQL, int start, string sql);

//验证create语句是否有效
string create_check(string SQL,int start);

//验证create table语句是否有效
string create_table(string SQL,int start);

//获得属性
string getattri(string temp,string sql);

//验证create index语句是否有效
string create_index(string SQL,int start);

//验证drop语句是否有效
string drop_check(string SQL,int start);

//验证drop database语句是否有效
string drop_database(string SQL,int start);

//验证drop table语句是否有效
string drop_table(string SQl,int start);

//验证drop index语句是否有效
string drop_index(string SQL,int start);

//将表达式转化为内部形式
string get_expression(string temp,string sql);

//获取表达式组的每个表达式
string get_each(string T,string sql,string condition);

//验证use语句是否有效
string use_check(string SQL,int start);


//验证quit语句是否有效
string quit_check(string SQL,int start);

#endif