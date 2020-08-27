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
	string Filename;  				//文件名
	int BlockOffSet;  				//块偏移量，记录当前位置 
	int time;                   	//LRU变量
	int valid;         				//是否有效，有效为1，无效为0
	int written;                    //是否修改（脏位） 
	char value[BlockSize + 1];		//记录value 
	
	void initialize();							//初始化 
	string getval(int begin, int end);			//在特定区域内取值 
	char getval(int index); 					//在特定位置取值 
};		

class BufferManager{
public:
	BufferManager();				//构造函数 
	~BufferManager();               //析构函数
	
	buffer BufferBlock[MaxBlockNum];        //buffer数组
	
	int whetherinbuffer(string filename, int BlockOffSet);      //判断该块是否在buffer中，如果是返回序号，否则返回-1 
	int getnum(string filename, int BlockOffSet);       		//获得该块在buffer中的序号，如果不在块中则将其导入块 
	void write_back(int num);            						//写回buffer 中的内容至文件中
	void read_Block(string filename, int BlockOffSet, int num);	//将某个块读到Buffer[num]中									//将块内容读入buffer[num]中 
	void write_Block(int num);									//Buffer[num].written = 1,同时按照LRU方法更新 
	void LRU(int num);             								//LRU算法 -- 最近最久未使用 
	int get_another_buffer();									//为了取得一个buffer位置 ；若无空buffer，按照LRU算法获取一个 
	int get_another_buffer(string filename);					//为了取得一个除文件名为filename以外的空buffer位置 
	insertPos GetPositionForInsert(Table& fileinformation);     //获得写入位置以便写入 
	int AddAnotherBlock(Table& files);							//在buffer中新开一个块来写入文件 
	int AddAnotherBlock(Index& indexs);							//在buffer中新开一个块 来写入index文件 
	void read_in_all(Table files);								//将整个文件读入buffer 
	void delete_file(string filename);							//删除文件时将相关块valid值置0 

};

#endif



