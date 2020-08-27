#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <string.h>
#include "MiniSQL.h"

//�������ж�statement�ĸ�������
inline TYPE getType(string s);

inline bool isUnique(string s);

void SQLstatement::outputinfo();

SQLstatement::SQLstatement(string SQL);

STATEMENT_TYPE SQLstatement::identify(string type_code);

Condition SQLstatement::genCondition(string exp);

RELATION_TYPE SQLstatement::getRelationType(string oper);

SQLstatement::SQLstatement(STATEMENT_TYPE type, string tableName);

SQLstatement::~SQLstatement();
//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
string Interpreter(string SQL);

//��֤select ����Ƿ���Ч
string select_check(string SQL, int start);

//�����������ļ����ÿһ��
string get_part(string temp, string sql, string kind);

//��֤execfile����Ƿ���Ч
string execfile_check(string SQL, int start);

//��֤insert ����Ƿ���Ч
string insert_check(string SQL, int start);

//��֤insert into values����Ƿ���Ч
string insert_into_values(string SQL, int start, string sql);

//��֤delete����Ƿ���Ч
string delete_check(string SQL, int start);

//��֤ delete from where ����Ƿ���Ч
string delete_from_where(string SQL, int start, string sql);

//��֤create����Ƿ���Ч
string create_check(string SQL,int start);

//��֤create table����Ƿ���Ч
string create_table(string SQL,int start);

//�������
string getattri(string temp,string sql);

//��֤create index����Ƿ���Ч
string create_index(string SQL,int start);

//��֤drop����Ƿ���Ч
string drop_check(string SQL,int start);

//��֤drop database����Ƿ���Ч
string drop_database(string SQL,int start);

//��֤drop table����Ƿ���Ч
string drop_table(string SQl,int start);

//��֤drop index����Ƿ���Ч
string drop_index(string SQL,int start);

//�����ʽת��Ϊ�ڲ���ʽ
string get_expression(string temp,string sql);

//��ȡ���ʽ���ÿ�����ʽ
string get_each(string T,string sql,string condition);

//��֤use����Ƿ���Ч
string use_check(string SQL,int start);


//��֤quit����Ƿ���Ч
string quit_check(string SQL,int start);

#endif