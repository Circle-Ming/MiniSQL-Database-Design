#include "RecordUtility.h"



RecordUtility::RecordUtility()
{
}


RecordUtility::~RecordUtility()
{
}



//////////////////////////////////////////////////////////////////////////////////
//	����ĳ����¼������
int RecordUtility::GetIndex(const vector<Attribute> & attrs, const Condition & cons)
{
	// �������е�����
	for (size_t i = 0; i < attrs.size(); i++)
	{
		// �ҵ������¼
		if (attrs[i].name.compare(cons.attribute.name)==0)
		{
			return i;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	��ѯ��¼�Ƿ�������������
bool RecordUtility::CheckCondition(const vector<Attribute> & attrs, const vector<Condition> & cons, const string * AllAttrs)
{
	// �������е�����
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
//	�����ڴ滺����
int RecordUtility::CopyBuffer(char * des, const char * src, int nStartPos, int len)
{
	int i = 0;
	for (; i < len; i++)
	{
		des[i + nStartPos] = src[i];
	}
	// ���ؽ�����λ��
	return i + nStartPos;
}

///////////////////////////////////////////////////////////////////////////////////////
//	��ѯ�����ͼ�¼��ƥ��
bool RecordUtility::CheckCondition(const Condition & con, const string & value, const TYPE type)
{
	// �������ͽ��о���ıȽϲ���
	switch (con.relationType)
	{
		// �����
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

		// ���
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

		// ����
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

		// ���ڵ���
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

		// С��
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

		// С�ڵ���
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
//	����ĳ�������Ƿ����
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
//	����ĳ�������Ƿ���Ч
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