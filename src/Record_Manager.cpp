#include <windows.h>
#include "Record_Manager.h"
#include "mybuffer.h"
#include "RecordUtility.h"


#define START_BIT				1
#define END_BIT					64

BufferManager bm;

//////////////////////////////////////////////////////////////////
//	针对Primary key 和 Nuique Key的查询操作
bool RecordManager::CheckValid(char * tempData, const Table & table)
{
	int nOffset = 1;

	// 遍历所有的属性，查找关键字是否存在
	for (size_t i = 0; i < table.attributes.size(); i++)
	{
		// 数据查长度
		int length = table.attributes[i].length;
		// 数据类型
		TYPE type = table.attributes[i].type;

		string strValue = GetStringValue(tempData + nOffset, length, type);

		// 如果是主键，则返回false
		if (table.attributes[i].isPrimaryKey)
		{
			// 如果这个键在表中已经存在，则返回错误
			if (CheckExist(tempData, nOffset, length, type, table))
			{

				cout << table.attributes[i].name << " is primary key. " << strValue << " can't be inserted" << endl;
				return false;
			}
		}
		// 如果是唯一，返回false
		else if (table.attributes[i].isUnique)
		{
			// 如果这个键在表中已经存在，则返回错误
			if (CheckExist(tempData, nOffset, length, type, table))
			{
				cout << table.attributes[i].name << " is unique. " << strValue << " can't be inserted" << endl;
				return false;
			}
		}

		// 移动偏移量
		nOffset += table.attributes[i].length;
	}

	return true;
}


///////////////////////////////////////////////////////////
//	插入一条记录
bool RecordManager::Insert(Table & table, const string & values, insertPos &insertPos)
{
	int nCurPos = 0, nStartPos = 0, nEndPos = 0;

	// 写入数据长度
	int nWriteLen = table.tupleLength + 1;

	// 根据长度开辟缓冲区
	char * Temp = new char[nWriteLen];

	// 初始化缓冲区
	memset(Temp, 0, sizeof(char) * nWriteLen);

	// 将整个文件读入buffer中
	bm.scanIn(table);

	bool visable = true;
	// 设置当前位置
	nCurPos = RecordUtility::CopyBuffer(Temp, (char *)&visable, nCurPos, sizeof(char));

	// 循环枚举所有的列，然后写入数据
	for (size_t i = 0; i < table.attributes.size(); i++)
	{
		nEndPos = values.find_first_of(',', nStartPos);
		string tempValue = values.substr(nStartPos, nEndPos - nStartPos);
		nStartPos = nEndPos + 1;

		// 根据不同的数据类型进行不同的处理
		switch (table.attributes[i].type)
		{
		case MYCHAR:
		{
			string tempString = tempValue.substr(1, tempValue.length() - 2);	// 去引号
			nCurPos = RecordUtility::CopyBuffer(Temp, tempString.c_str(), nCurPos, table.attributes[i].length);	// 可能存在越界
			break;
		}
		case MYINT:
		{
			int tempInt = std::stoi(tempValue);
			nCurPos = RecordUtility::CopyBuffer(Temp, (char *)&tempInt, nCurPos, sizeof(int));
			break;
		}
		case MYFLOAT:
		{
			float tempFloat = std::stof(tempValue);
			nCurPos = RecordUtility::CopyBuffer(Temp, (char *)&tempFloat, nCurPos, sizeof(int));
			break;
		}
		default:
			cout << "error in insertValue!" << endl;
			break;
		}
	}

	// 保存传入的数据，然后数据总数量+1
	if (CheckValid(Temp, table))
	{
		insertPos = bm.getInsertPosition(table);

		// 数据写入到buffer中
		memcpy(&bm.bufferBlock[insertPos.bufferNUM].value[insertPos.position], Temp, nWriteLen);

		// 数据总数量+1
		table.recordNum++;

		// 释放空间
		delete Temp;

		return true;
	}
	else
	{
		// 释放空间
		delete Temp;

		return false;
	}
}

/////////////////////////////////////////////////////////////
//	删除表格
bool RecordManager::DeleteTable(string & tableName)
{
	// 生成表和表文件的名字
	string strTableFileName = "./bm/" + tableName + ".table";
	string strTableName = tableName + ".table";

	// 将数据表设置为不可用
	bm.setInvalid(strTableName);
	// 删除数据文件
	::DeleteFileA(strTableFileName.c_str());

	return true;
}

/////////////////////////////////////////////////////////////
//	无条件删除表中记录
int RecordManager::Delete(Table & table)
{
	// 生成表名字
	string strTableName = table.name + ".table";

	// 将整个文件读入buffer中
	bm.scanIn(table);

	// 需要删除的的长度
	int nDelLen = table.tupleLength + 1;
	// 删除的行数
	int nDelCount = 0;

	// 循环枚举Buffer，找到符合条件的记录
	int buf_offset = 0;
	while (buf_offset < MAXBLOCKNUMBER)
	{
		// 属于表的块
		if (bm.bufferBlock[buf_offset].filename.compare(strTableName) == 0)
		{
			for (int i = 0; i <= BLOCKSIZE - nDelLen; i += nDelLen)
			{
				int vbit = bm.bufferBlock[buf_offset].value[i];	// 取得visibleBit

				if (vbit == END_BIT)
				{
					break;
				}

				if (vbit == START_BIT)
				{
					// 将本记录设置为不可见
					bm.bufferBlock[buf_offset].value[i] = (char)false;
					// 删除数量+1
					nDelCount++;
				}

			}
		}

		buf_offset++;
	}

	// 表中记录集的总数量减去删除的记录数量
	table.recordNum -= nDelCount;

	// 返回删除记录的数量
	return nDelCount;
}


/////////////////////////////////////////////////////////////
//	按照条件删除表中记录
int RecordManager::DeleteByCondition(Table & table, vector<Condition> & conditions)
{
	// 生成表名字
	string strTableName = table.name + ".table";

	// 将整个文件读入buffer中
	bm.scanIn(table);

	// 需要删除的的长度
	int nDelLen = table.tupleLength + 1;
	// 删除的行数
	int nDelCount = 0;

	// 按照属性列的数量申请空间
	string * allAttribute = new string[table.attriNum];

	// 循环枚举Buffer，找到符合条件的记录
	int buf_offset = 0;
	while (buf_offset < MAXBLOCKNUMBER)
	{
		// 找到要查询的表
		if (bm.bufferBlock[buf_offset].filename.compare(strTableName) == 0)
		{
			// 遍历
			for (int i = 0; i <= BLOCKSIZE - nDelLen; i += nDelLen)
			{
				int vbit = bm.bufferBlock[buf_offset].value[i];

				if (vbit == END_BIT)
				{
					break;
				}

				if (vbit == START_BIT)
				{
					int nOffset = 1;	// 跳过visibleBit

										// 依次取出每个属性的值
					for (int j = 0; j < table.attriNum; j++)
					{
						allAttribute[j] = GetStringValue(bm.bufferBlock[buf_offset].value + i + nOffset, table.attributes[i].length, table.attributes[i].type);
						// 获取下一个属性的偏移
						nOffset += table.attributes[j].length;
					}
					// 检查是否符合条件
					if (RecordUtility::CheckCondition(table.attributes, conditions, allAttribute))
					{
						// set visable bit to 0(false);
						bm.bufferBlock[buf_offset].value[i] = (char)false;
						nDelCount++;
					}
				}
			}
		}

		buf_offset++;
	}

	// 记录的总数量减去删除行数
	table.recordNum -= nDelCount;

	// 返回删除记录的数量
	return nDelCount;
}


/////////////////////////////////////////////////////////////////////////////////////
//	根据条件查询记录
int RecordManager::SearchByCondition(Table & table, const vector<Attribute> & att, const vector<Condition> & conditions)
{
	// 生成表名字
	string strTableName = table.name + ".table";

	// 清空列表
	m_Attributes.clear();	

	int nReadLen = table.tupleLength + 1;
	
	// 查询记录的个数
	int nSearchCount = 0;						
	
	// 查询属性列表
	vector<Attribute> attrs;		

	// 将整个文件读入buffer中
	bm.scanIn(table);

	// 如果是*则是全选记录
	// 获取用户查询的行数
	if (att[0].name.compare("*")!=0)		
	{
		nSearchCount = att.size();
		attrs = att;
	}
	// 如果是全选记录
	else
	{
		nSearchCount = table.attributes.size();
		attrs = table.attributes;		
	}


	// 值列表
	vector<string> * cluster = new vector<string>[nSearchCount];

	// 用于存放当前行的值
	string * allAttrValuestemp = new string[table.attriNum];

	int buf_offset = 0;
	while (buf_offset < MAXBLOCKNUMBER)
	{
		// 找到要查询的表
		if (bm.bufferBlock[buf_offset].filename.compare(strTableName) == 0)
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - nReadLen; blockIndex += nReadLen)	
			{
				// 取得visibleBit
				int vbit = bm.bufferBlock[buf_offset].value[blockIndex];	
				if (vbit == START_BIT)
				{
					// 跳过visibleBit
					int positionOffset = 1;	
					// 选择属性的下标
					int count = 0;	
					for (int i = 0; i < table.attriNum; i++)
					{
						string value = GetStringValue(bm.bufferBlock[buf_offset].value + blockIndex + positionOffset, table.attributes[i].length, table.attributes[i].type);
						allAttrValuestemp[i] = value;	// 暂存当前遍历的tuple的属性值
						if (RecordUtility::CheckAttriExist(attrs, table.attributes[i]))
						{
							// 记录是第几个需要的属性
							cluster[count].push_back(value);/*把值放入列表*/
							count++;
						}
						positionOffset += table.attributes[i].length;
					}

					// 判断读取的记录是否符合条件	
					if (!RecordUtility::CheckCondition(table.attributes, conditions, allAttrValuestemp))			
					{						
						// 提出不满足条件的记录
						for (int i = 0; i < nSearchCount; i++)
						{
							cluster[i].pop_back();
						}
					}
				}
				else if (vbit == END_BIT)
				{
					break;
				}
			}
		}

		buf_offset++;
	}

	// 选出用户查询的记录
	if (cluster[0].size())	
	{
		// 保存记录
		for (int i = 0; i < nSearchCount; i++)
		{
			m_Attributes.insert(std::pair<string, vector<string> >(attrs[i].name, cluster[i]));
		}

		delete[] allAttrValuestemp;

		// 返回记录数量
		return cluster[0].size();
	}
	else
	{
		delete[] cluster;
		delete[] allAttrValuestemp;

		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
//	无条件查询记录
int RecordManager::Search(Table & table, const vector<Attribute> & att)
{
	// 生成表名字
	string strTableName = table.name + ".table";

	// 清空列表
	m_Attributes.clear();

	int nReadLen = table.tupleLength + 1;

	// 查询记录的个数
	int nSearchCount = 0;

	// 查询属性列表
	vector<Attribute> attrs;

	// 将整个文件读入buffer中
	bm.scanIn(table);

	// 如果是*则是全选记录
	// 获取用户查询的行数
	if (att[0].name.compare("*") != 0)
	{
		nSearchCount = att.size();
		attrs = att;
	}
	// 如果是全选记录
	else
	{
		nSearchCount = table.attributes.size();
		attrs = table.attributes;
	}

	// 值列表
	vector<string> * cluster = new vector<string>[nSearchCount];

	int buf_offset = 0;
	while (buf_offset < MAXBLOCKNUMBER)
	{
		// 找到要查询的表
		if (bm.bufferBlock[buf_offset].filename.compare(strTableName) == 0)
		{
			// 遍历所有的块路进行搜索
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - nReadLen; blockIndex += nReadLen)
			{
				int visibleBit = bm.bufferBlock[buf_offset].value[blockIndex];	// 取得visibleBit
				if (visibleBit == START_BIT)
				{
					int nOffset = 1;	// 跳过visibleBit
					int count = 0;	// 选择属性的下标
					for (int i = 0; i < table.attriNum; i++)
					{
						string value = GetStringValue(bm.bufferBlock[buf_offset].value + blockIndex + nOffset, table.attributes[i].length, table.attributes[i].type);
						if (RecordUtility::CheckAttriExist(attrs, table.attributes[i]))
						{
							// 将用户查询的属性保存下来
							cluster[count].push_back(value);/*把值放入列表*/
							count++;
						}
						nOffset += table.attributes[i].length;
					}
				}
				else if (visibleBit == END_BIT)
				{
					break;
				}
			}
		}

		buf_offset++;
	}

	// 选出用户查询的记录
	if (cluster[0].size())
	{
		// 保存记录
		for (int i = 0; i < nSearchCount; i++)
		{
			m_Attributes.insert(std::pair<string, vector<string> >(attrs[i].name, cluster[i]));
		}

		// 返回记录数量
		return cluster[0].size();
	}
	else
	{
		delete[] cluster;
		return 0;
	}
}

////////////////////////////////////////////////////////////////////
//	查询某个关键字是否存在
bool RecordManager::CheckExist(char * data, int nCurPos, int len, TYPE type, const Table & table)
{
	// 生成表名字
	string strTableName = table.name + ".table";

	// 首先将关键字转换为字符串形式
	string strKey = GetStringValue(data + nCurPos, len, type);
	int readSize = table.tupleLength + 1;

	// 遍历整个数据Buffer
	for (int buf_offset = 0; buf_offset < MAXBLOCKNUMBER; buf_offset++)
	{
		// 找到要查询的表
		if (bm.bufferBlock[buf_offset].filename.compare(strTableName) == 0)
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - readSize; blockIndex += readSize)
			{
				int vbit = bm.bufferBlock[buf_offset].value[blockIndex];
				if (vbit)
				{
					// 取出每一个记录进行查找
					string existValue = GetStringValue(bm.bufferBlock[buf_offset].value + blockIndex + nCurPos, len, type);
					// 如果两者相等则是查询到
					if (existValue.compare(strKey)==0)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
//	根据不同的数据类型，按照不同的字符串转换方式进行转换
string RecordManager::GetStringValue(const char * source, int length, TYPE type)
{
	string value;

	// 根据数据类型的不同，进行不同的转换
	switch (type)
	{
		// 字符串类型
	case MYCHAR:
		value = string(source);
		break;
		// 证书类型
	case MYINT:
		value = to_string(*((int *)source));
		break;
		// 浮点类型
	case MYFLOAT:
		value = to_string(*((float *)source));
		break;
	default:
		cout << " error!" << endl;
		break;
	}

	return value;
}



//////////////////////////////////////////////////////////
//	显示记录
void RecordManager::ShowRecord(int tupleCount)
{
	for (auto& result : m_Attributes) {
		cout << result.first << "\t\t";
	}
	cout << endl;
	for (int i = 0; i < tupleCount; i++)
	{
		for (auto& result : m_Attributes) {
			if(result.second[i].length()>5){
				cout << result.second[i] << "\t";
			}else{
				cout << result.second[i] << "\t\t";
			}
		}
		cout << endl;
	}
}