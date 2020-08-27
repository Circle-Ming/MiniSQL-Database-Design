#pragma once
#include<afx.h>
#include"Minisql.h"
#include"mybuffer.h"
#include"Record_Manager.h"
template<class T>
class Node
{
public:
	Node(int address, int length);//���캯��
	~Node();
	void SetFile(float* p);//���������ת��Ϊ�����ļ��������ַ���
	void GetFile(float* p);//���ַ��������������
	bool isLeaf;//�ж��Ƿ�ΪҶ�ӽڵ�
	int Pointer[3][2];//ָ�룬�������ļ��е����ݣ�ָ���������ͱ�ǵĵ�ַ
	int Father;//ָ�򸸽ڵ��ָ��
	int Next;//ָ����һ���ڵ㣬����Ҷ�ӽڵ���������

	bool deleteTag;//ɾ�����
	int KeyCount;//�Ѿ�����Ĺؼ���ֵ����
	int Address;//�����ַ

	int KeyLength;//�ؼ������͵ĳ��ȣ�����char��Ҫ�涨����֮��������Ϊ1
	T* Key[3];//�ؼ���	
};



class BTree :public CObject
{
public:
	BTree() {}
	~BTree();


	bool inUse;
	int Root;//���ڵ�ĵ�ַ
	int MaxOffset;//���ĵ�ַ��֮��Ϊ�հ׵Ŀ��õ�ַ

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
