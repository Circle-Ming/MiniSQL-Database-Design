#include "RecordUtility.h"



RecordUtility::RecordUtility()
{
}


RecordUtility::~RecordUtility()
{
}



//////////////////////////////////////////////////////////////////////////////////
//	返回某个记录的索引
int RecordUtility::GetIndex(const vector<Attribute> & attrs, const Condition & cons)
{
	// 遍历所有的索引
	for (size_t i = 0; i < attrs.size(); i++)
	{
		// 找到这个记录
		if (attrs[i].name.compare(cons.attribute.name)==0)
		{
			return i;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	查询记录是否满足搜索条件
bool RecordUtility::CheckCondition(const vector<Attribute> & attrs, const vector<Condition> & cons, const string * AllAttrs)
{
	// 遍历所有的条件
	for (size_t i = 0; i < cons.size(); i++)
	{
		int attrIndex = GetIndex(attrs, cons[i]);
		if ((!CheckCondition(cons[i], AllAttrs[attrIndex], attrs[attrIndex].type)))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
//	拷贝内存缓冲区
int RecordUtility::CopyBuffer(char * des, const char * src, int nStartPos, int len)
{
	int i = 0;
	for (; i < len; i++)
	{
		des[i + nStartPos] = src[i];
	}
	// 返回结束的位置
	return i + nStartPos;
}

///////////////////////////////////////////////////////////////////////////////////////
//	查询条件和记录的匹配
bool RecordUtility::CheckCondition(const Condition & con, const string & value, const TYPE type)
{
	// 根据类型进行具体的比较操作
	switch (con.relationType)
	{
		// 不相等
	case NOT_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) != atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) != atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value != con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;

		// 相等
	case EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) == atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) == atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value == con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;

		// 大于
	case GREATER:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) > atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) > atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value > con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;

		// 大于等于
	case GREATER_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) >= atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) >= atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value >= con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;

		// 小于
	case SMALLER:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) < atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) < atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value < con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;

		// 小于等于
	case SMALLER_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) <= atoi(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) <= atof(con.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value <= con.value.substr(1, con.value.length() - 2))
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	default:break;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////
//	查找某个属性是否存在
bool RecordUtility::CheckAttriExist(const vector<Attribute> & attrs, const Attribute & attr)
{
	for (size_t i = 0; i < attrs.size(); i++)
	{
		if (attrs[i].name.compare(attr.name) == 0)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
//	查找某个条件是否有效
int RecordUtility::CheckConditionExist(const vector<Condition> & cons, const string & attrName)
{
	for (size_t i = 0; i < cons.size(); i++)
	{
		if (cons[i].attribute.name.compare(attrName) == 0)
		{
			return i + 1;
		}
	}
	return 0;
}