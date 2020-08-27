#include "mybuffer.h"
#define EMPTY '@'

void buffer::initialize() {
	Filename = "NULL";
	BlockOffSet = 0;
	time = 0;
	valid = 0;
	written = 0;
	
	int i = 0;
	while(i < BlockSize){
		value[i] = '@';
		i++;
	}
	value[BlockSize] = '\0';
	
}

string buffer::getval(int begin , int end){
	int temp;
	int i;
	string str = "";
	
	temp = (start > 0 && start < = end && end <= BlockSize);
	
	if(temp){
		for(i = start; i < end; i++){
			str += value[i];
		}
		
			return str;
	}
	
	return NULL;
}

char buffer::getvalue(int index){
	int temp;
	
	temp = (index >=0 && index < BlockSize);
	
	if(temp) return value[index];
	
	return '\0';
} 

BufferManager::BufferManager(){
	int i = 0;
	
	while(i < MaxBlockNum){
		BufferBlock[i].initialize();
		i++; 
	}
}


BufferManager::~BufferManager(){
	int i = 0;
	
	while(i < MaxBlockNum){
		write_back(i);
		i++; 
	}
}

int BufferManager::whetherinbuffer(string filename, int BlockOffSet){
	int num = 0;
	int temp = 0;
	
	while(num < MaxBlockNum){
		temp = (BufferBlock[num].filename == filename && BufferBlock[num].BlockOffSet == BlockOffSet);
		if(temp) return num;
		num++;
	}
	return -1;
}

int BufferManager::getnum(string filename, int BlockOffSet){
	int num;
	
	num = whetherinbuffer(filename, BlockOffSet);
	
	if(num == -1){
		num = get_another_buffer(filename);
		read_in(filename, BlockOffSet, num);	
	}
	
	return num;
}

void BufferManager::read_Block(string filename, int BlockOffSet, int num){
	fstream fs(("./bm/" + filename).c_str(), ios::binary | ios::in);             //ע���ļ���ʽ������
	BufferBlock[num].filename = filename;
	BufferBlock[num].BlockOffSet = BlockOffSet;
	BufferBlock[num].valid = 1;
	BufferBlock[num].written = 0;

	fs.seekp(BlockSize * BlockOffSet, fs.beg);
	fs.read(BufferBlock[num].value, BlockSize);
	fs.close();
	 
}

void BufferManager::write_back(int num);{
	int a,b;
	
	a = BufferBlock[num].written;
	b = BufferBlock[num].valid;
	
	string filename = BufferBlock[num].filename;
	
	if(a == 0 && b == 0)  return;   														//����δ��ʹ��Ҳδ��д�� 
	if((a == 0 && b == 1) || (a == 1 && b == 0)){											//����ʹ�û�д�� 
		fstream fs(("./bm/" + filename).c_str(), ios::binary | ios::app | ios::out);
		fs.seekp(BlockSize * BufferBlock[num].BlockOffSet, fs.beg);
		fs.write(BufferBlock[num].value,BlockSize);
		fs.close();
	}
	else{																					//�ȱ�ʹ��Ҳ��д�� 
		fstream fs(("./bm/" + filename).c_str(), ios::binary | ios::out);
		fs.seekp(BlockSize * BufferBlock[num].BlockOffSet, fs.beg);
		fs.write(BufferBlock[num].value,BlockSize);
		fs.close();
	} 
	
	BufferBlock[num].initialize();															//��ʼ����bufferblock 
}

void BufferManager::write_Block(int num){
	BufferBlock[num].written = 1;
	
	LRU(num); 
}

void BufferManager::LRU(int num){
	int i = 0;
	
	/*
	LRU�㷨���������δʹ��
	ÿ�����������ݣ�����Ԫ��ʱ�����������Ԫ��ʱ�����Ϊ0 
	*/
	while(i < BlockSize){
		if(i == num){
			BufferBlock[num].time = 0;
			BufferBlock[num].valid = 1;
		}
		
		else BufferBlock[num].time++;
		
		i++;
	}
} 

int BufferManager::get_another_buffer(){
	int num,maxtime,i;
	
	num = 0;
	i = 0;
	maxtime = -1;
	
	while(i < MaxBlockNum){
		if(BufferBlock[i].valid == 0){    //�ҵ�һ����buffer
			BufferBlock[i].initialize();
			BufferBlock[i].valid = 1; 
			
			return i;
		}
		else if(maxtime < BufferBlock[i].time){
			num = i;
			maxtime = BufferBlock[num].time;
		}
		
		i++
	}
	
	write_Back(num);        //���� buffer �е�����д���ļ���������λ��
	BufferBlock[num].valid = 1;
	
	return num; 
}

int BufferManager::get_another_buffer(string filename){
	int num,maxtime,i;
	
	num = 0;
	i = 0;
	maxtime = -1;
	
	while(i < MaxBlockNum){
		if(BufferBlock[i].valid == 0){    //�ҵ�һ����buffer
			//BufferBlock[i].initialize();
			BufferBlock[i].valid = 1; 
			
			return i;
		}
		else if(maxtime < BufferBlock[i].time && BufferBlock[i].filename != filename){
			num = i;
			maxtime = BufferBlock[num].time;
		}
		
		i++
	}
	
	if(num == -1){
		cout << "We are out of space !"<<endl;
		return -1;
	}
	
	write_Back(num);        //���� buffer �е�����д���ļ���������λ��
	BufferBlock[num].valid = 1;
	
	return num; 
} 

insertPos BufferManager::GetPositionForInsert(Table & fileinformation){
	insertPos insertp;
	int Buff_long, BlockOffSet, num;
	int OffSet = 0;
	int pos;
	char isEmpty;
	
	
	if(!fileinformation.blockNum){		//���file.blocknum == 0 
		insertp.bufferNUM = addBlockInFile(fileinformation);
		insertp.position = 0;
		return insertp;
	}
	
	string filename = fileinformation.name + ".table";
	
	Buff_long = fileinformation.tupleLength + 1;
	BlockOffSet = fileinformation.blockNum -1;
	num = whetherinbuffer(filename, BlockOffSet);    //Ѱ�Ҹÿ���buffer�е�λ�� 
	
	if(num < 0){            //num == -1 
		num = get_another_buffer();
		read_Block(filename, BlockOffSet, num);
	}
	
	const int record_num = BlockSize / Buff_long;
	
	while(OffSet < (BlockSize / Buff_long) ){
		pos = OffSet * Buff_long;       //get the position
		isEmpty = BufferBlock[num].value[pos];
		
		if(isEmpty == '@')	{
			insertp.bufferNUM = num;			//num��Ϊ�ҵ���λ��ֵ 
			insertp.position = pos;
			
			return insertp;
		}
		
		OffSet++;       //��δ�ҵ���ƫ������1 
	}
	
	insertp.bufferNUM = AddAnotherBlock(fileinformation);
	insertp.position = 0;
	
	return insertp;
	
} 

int BufferManager::AddAnotherBlock(Table& files){
	int num;
	
	num = get_another_buffer();
	
	if(num == -1){                             //�ռ䲻��  
		cout << "Check the space !" << endl;
		return -1;
	}
	
	//���п���ռ����ʼ�� 
	BufferBlock[num].initialize();
	BufferBlock[num].filename = files.name + ".table";
	BufferBlock[num].written = 1;
	BufferBlock[num].BlockOffSet = files.blockNum;
	BufferBlock[num].valid = 1;
	files.blockNum++;
	
	return num;
}

int BuffManager::AddAnotherBlock(Index& indexs){
	int num;
	
	string filename = indexs.index_name + ".index";
	
	num = get_another_buffer(filename);
	
	if(num == -1){							
		cout << "Check the space !" << endl;
		return -1;
	}
	
	BufferBlock[num].initialize();
	BufferBlock[num].written = 1;
	BufferBlock[num].filename = filename;
	BufferBlock[num].valid = 1;
	BufferBlock[num].BlockOffSet = indexs.IndexOffset;
	indexs.IndexOffset++;
	
	return num;	
}

void BufferManager::scanIn(Table files){
	int BlockSetOff = 0;
	int temp = 0;
	int num;
	
	string filename = files.name + ".table";     //�ļ��� + table 
	fstream fin(("./bm/" + filename).c_str(), ios::binary | ios::in);      
	
	while(BlockSetOff < files.blockNum){     //ѭ������ 
		temp = whetherinbuffer(filename, BlockOfSet);
		
		if (temp == -1){        		//������buffer�� 
			num = get_another_buffer(filename);
			read_Block(filename, BlockOffSet, num);
		}
		
		BlockOffSet++;
	}
	
	fin.close();
}

void BufferManager::delete_file(string filename){
	int i;
	
	while(i < MaxBlockNum){
		if(filename == BufferBlock[i].filename){
			BufferBlock[i].valid = 0;
			BufferBlock[i].written = 0;
		}
		i++;
	}
}
























