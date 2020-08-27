#ifndef __BufferManager_H__
#define __BufferManager_H__

#define BlockSize 4096
#define MaxBlockNum 500

#include<string>

class buffer{
	friend class BuffManager;
	buffer(){} 
	~buffer(){}
	
	
public:
	string Filename;  				//�ļ���
	int BlockOffSet;  				//��ƫ��������¼��ǰλ�� 
	int time;                   	//LRU����
	int valid;         				//�Ƿ���Ч����ЧΪ1����ЧΪ0
	int written;                    //�Ƿ��޸ģ���λ�� 
	char value[BlockSize + 1];		//��¼value 
	
	void initialize();							//��ʼ�� 
	string getval(int begin, int end);			//���ض�������ȡֵ 
	char getval(int index); 					//���ض�λ��ȡֵ 
};		

class BufferManager{
public:
	BufferManager();				//���캯�� 
	~BufferManager();               //��������
	
	buffer BufferBlock[MaxBlockNum];        //buffer����
	
	int whetherinbuffer(string filename, int BlockOffSet);      //�жϸÿ��Ƿ���buffer�У�����Ƿ�����ţ����򷵻�-1 
	int getnum(string filename, int BlockOffSet);       		//��øÿ���buffer�е���ţ�������ڿ������䵼��� 
	void write_back(int num);            						//д��buffer �е��������ļ���
	void read_Block(string filename, int BlockOffSet, int num);	//��ĳ�������Buffer[num]��									//�������ݶ���buffer[num]�� 
	void write_Block(int num);									//Buffer[num].written = 1,ͬʱ����LRU�������� 
	void LRU(int num);             								//LRU�㷨 -- ������δʹ�� 
	int get_another_buffer();									//Ϊ��ȡ��һ��bufferλ�� �����޿�buffer������LRU�㷨��ȡһ�� 
	int get_another_buffer(string filename);					//Ϊ��ȡ��һ�����ļ���Ϊfilename����Ŀ�bufferλ�� 
	insertPos GetPositionForInsert(Table& fileinformation);     //���д��λ���Ա�д�� 
	int AddAnotherBlock(Table& files);							//��buffer���¿�һ������д���ļ� 
	int AddAnotherBlock(Index& indexs);							//��buffer���¿�һ���� ��д��index�ļ� 
	void read_in_all(Table files);								//�������ļ�����buffer 
	void delete_file(string filename);							//ɾ���ļ�ʱ����ؿ�validֵ��0 

};

#endif



