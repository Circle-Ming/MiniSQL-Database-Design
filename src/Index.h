#pragma once
#include<afx.h>
#include"Minisql.h"
#include"mybuffer.h"
#include"Record_Manager.h"
template<class T>
class Node
{
public:
	Node(int address, int length);//构造函数
	~Node();
	void SetFile(float* p);//将类的属性转化为易于文件操作的字符串
	void GetFile(float* p);//将字符串填入各个属性
	bool isLeaf;//判断是否为叶子节点
	int Pointer[3][2];//指针，由于是文件中的内容，指向两个整型标记的地址
	int Father;//指向父节点的指针
	int Next;//指向下一个节点，仅在叶子节点中起作用

	bool deleteTag;//删除标记
	int KeyCount;//已经填入的关键字值个数
	int Address;//自身地址

	int KeyLength;//关键字类型的长度，除了char需要规定长度之外其他都为1
	T* Key[3];//关键字	
};



class BTree :public CObject
{
public:
	BTree() {}
	~BTree();


	bool inUse;
	int Root;//根节点的地址
	int MaxOffset;//最大的地址，之后为空白的可用地址

};

class Index {
private:
	AttributeValuesMap attri;
public:

	void CreateTree(string IndexName, Attribute attribute);
	void CreateIndex(string IndexName);
	void InsertNode(Table& t, string s, int block, int position);
	void DeleteNode(Table & table, const vector<Attribute>& attributes, const vector<Condition>& conditions);
	void DropIndex(string IndexName);
	void DropAll(Table& table);
	void FindNode(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions);
};

template<class T>
inline Node<T>::Node(int address, int length) {
	isLeaf = false;
	deleteTag = false;
	KeyCount = 0;
	Address = address;
	KeyLength = length;
	for (int i = 0; i < 3; i++) {
		Key[i] = new T[KeyLength];
		for (int j = 0; j < KeyLength; j++)
			Key[i][j] = 0;
		for (int j = 0; j < 2; j++)
			Pointer[i][j] = 0;

	}
	Father = 0;
	Next = 0;
}

template<class T>
inline void Node<T>::SetFile(int * p) {
	p[0] = isLeaf;
	p[1] = deleteTag;
	p[2] = KeyCount;
	p[3] = Address;
	p[4] = KeyLength;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < KeyLength; j++)
			p[5 + i * KeyLength + j] = Key[i][j];
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++)
			p[5 + 3 * KeyLength + 2 * i + j] = Pointer[i][j];
	}
	p[11 + 3 * KeyLength] = Father;
	p[12 + 3 * KeyLength] = Next;
}

template<class T>
inline void Node<T>::GetFile(int * p) {
	isLeaf = p[0];
	delete = Tagp[1];
	KeyCount = p[2];
	Address = p[3];
	KeyLength = p[4];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < KeyLength; j++)
			Key[i][j] = p[5 + i * KeyLength + j];
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++)
			Pointer[i][j] = p[5 + 3 * KeyLength + 2 * i + j];
	}
	Father = p[11 + 3 * KeyLength];
	Next = p[12 + 3 * KeyLength];
}
