#include"Index.h"
#include<iomanip>
BufferManager bm;
RecordManager rm;
void Index::CreateTree(string IndexName, Attribute attribute)
{
	CFileFind f;
	string s = IndexName.c_str();
	s = "./im/" + s;
	LPCSTR t = s.c_str();
	bool exist = f.FindFile(t);
	if (exist)
		cout << "The index name has already exist!" << endl;
	else if (attribute.isUnique == 0)
		cout << "Can't create an index here, because thhe attribute is not unique!" << endl;
	else {
		CFile file(t, CFile::modeCreate || CFile::modeReadWrite);
		BTree b;
		b.inUse = false;
		b.Root = 1;
		b.MaxOffset = 1;


		
		int length;
		attribute.indexName = IndexName;
		if (attribute.type == MYINT) {
			length = 1;
			float* temp;
			temp = new int[19];
			temp[0] = b.inUse;
			temp[1] = b.Root;
			temp[2] = b.MaxOffset;
			Node<int> n(1, 1);
			n.isLeaf = true;
			n.SetFile(temp + 3);
		}
		else if (attribute.type == MYFLOAT) {
			length = 1;
			temp = new float[19];
			temp[0] = b.inUse;
			temp[1] = b.Root;
			temp[2] = b.MaxOffset;
			Node<float>n(1, 1);
			n.isLeaf = true;
			n.SetFile(temp + 3);
		}
		else if (attribute.type == MYCHAR) {
			length = attribute.length;
			temp = new float[3 * length + 16];
			temp[0] = b.inUse;
			temp[1] = b.Root;
			temp[2] = b.MaxOffset;
			Node<char>n(1, length);
			n.isLeaf = true;
			n.SetFile(temp + 3);
		}
		else cout << "Wrong Type!" << endl;
		file.Write(temp, (3 * length + 16)*4);
		file.Close();
	}
}

void Index::CreateIndex(string IndexName)
{
	CFile file;
	IndexName = "./im/" + IndexName;
	const char* s = IndexName.c_str();
	bool open = file.Open(s, CFile::modeReadWrite);
	if (!open) {
		cout << "Cannot create index!" << endl;
		return;
	}
	float* temp;
	file.Read(temp, 4);
	if (*temp == 1) cout << "Index Exist" << endl;
	else {
		temp[0] = 1;
		file.Write(temp, 4);
		cout << "index " << IndexName << " create success" << endl;
	}
}

void Index::InsertNode(Table &Table, string s, int block, int position)
{
	BTree b;

	int slength;
	string temp;
	string indexName;
	slength = s.length();
	int i = 0;
	int j;
	vector<Attribute>::iterator iter;
	for (iter = Table.attributes.begin(); iter != Table.attributes.end(); iter++)
	{
		if (iter->isUnique == 0)
		{
			i = s.find_first_of(',', i) + 1;

		}
		else
		{
			temp = s.substr(i, (s.find_first_of(',', i) - i));
			const char* v = temp.c_str();
			if ((indexName = iter->indexName) == "") {
				return;
			}
			else {
				CFile file;
				indexName = "./im/" + indexName;
				file.Open(indexName.c_str(), CFile::modeReadWrite);
				float f[3];
				file.Read(f, 12);
				b.inUse = f[0];
				b.Root = f[1];
				b.MaxOffset = f[2];

				if (iter->type == MYINT) {
					int value = atoi(v);
					Node<int> n(b.Root, 1);
					float p[19];
					file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
					file.Read(p, 19*4);
					n.GetFile(p);
					while (!n.isLeaf) {
						for (j = 0; j < n.KeyCount; j++) {
							if (*n.Key[j] == value) {
								cout << "Key exist" << endl;
								return;
							}
							if (*n.Key[j] > value) break;
						}
						int offset = n.Pointer[j][1];
						file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
						file.Read(p, 19*4);
						n.GetFile(p);
					}
					if (n.KeyCount < 2) {
						n.Key[n.KeyCount][0] = value;
						n.Pointer[n.KeyCount][0] = block;
						n.Pointer[n.KeyCount][1] = position;
						n.KeyCount++;
						n.SetFile(p);
						file.Seek(2 + (n.Address - 1) * 19, CFile::begin);
						file.Write(p, 19*4);
						return;
					}


					while (n.KeyCount >= 2) {
						Node<int> newNode((b.MaxOffset - 1) * 19 + 2, 1);
						b.MaxOffset++;

						newNode.Key[0][0] = value;
						newNode.Pointer[0][0] = block;
						newNode.Pointer[0][1] = position;
						newNode.KeyCount = 1;
						newNode.isLeaf = n.isLeaf;
						if (newNode.isLeaf)
							newNode.Next = n.Next;
						if (n.Father == 0) {
							Node<int> newRoot((b.MaxOffset - 1) * 19 + 2, 1);
							b.MaxOffset++;
							n.Father = newRoot.Address;
							newRoot.Key[0][0] = value;
							newRoot.KeyCount = 1;
							newRoot.Pointer[0][1] = n.Address;
							newRoot.Pointer[1][1] = newNode.Address;
							b.Root = newRoot.Address;
						}
						newNode.Father = n.Father;
						if (n.isLeaf)
							n.Next = newNode.Address;

						n.SetFile(p);
						file.Seek(11 + (n.Address - 1) * 19*4, CFile::begin);
						file.Write(p, 19*4);
						newNode.SetFile(p);
						file.Seek(11 + (newNode.Address - 1) * 19*4, CFile::begin);
						file.Write(p, 19*4);

						file.Seek((n.Father - 1) * 19*4 + 11, CFile::begin);
						file.Read(p, 19*4);
						n.GetFile(p);
					}
					n.Key[n.KeyCount][0] = value;
					n.Pointer[n.KeyCount][0] = block;
					n.Pointer[n.KeyCount][1] = position;
					n.KeyCount++;
					n.SetFile(p);
					file.Seek(11 + (n.Address - 1) * 19*4, CFile::begin);
					file.Write(p, 19*4);
					return;
				}
				else if (iter->type == MYFLOAT) {
					float value = atof(v);
					Node<float> n(b.Root, 1);
					float p[19];
					file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
					file.Read(p, 19*4);
					n.GetFile(p);
					while (!n.isLeaf) {
						for (j = 0; j < n.KeyCount; j++) {
							if (*n.Key[j] == value) {
								cout << "Key exist" << endl;
								return;
							}
							if (*n.Key[j] > value) break;
						}
						int offset = n.Pointer[j][1];
						file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
						file.Read(p, 19*4);
						n.GetFile(p);
					}
					if (n.KeyCount < 2) {
						n.Key[n.KeyCount][0] = value;
						n.Pointer[n.KeyCount][0] = block;
						n.Pointer[n.KeyCount][1] = position;
						n.KeyCount++;
						n.SetFile(p);
						file.Seek(11 + (n.Address - 1) * 19*4, CFile::begin);
						file.Write(p, 19*4);
						return;
					}


					while (n.KeyCount >= 2) {
						Node<float> newNode((b.MaxOffset - 1) * 19 + 2, 1);
						b.MaxOffset++;

						newNode.Key[0][0] = value;
						newNode.Pointer[0][0] = block;
						newNode.Pointer[0][1] = position;
						newNode.KeyCount = 1;
						newNode.isLeaf = n.isLeaf;
						if (newNode.isLeaf)
							newNode.Next = n.Next;
						if (n.Father == 0) {
							Node<float> newRoot((b.MaxOffset - 1) * 19 + 2, 1);
							b.MaxOffset++;
							n.Father = newRoot.Address;
							newRoot.Key[0][0] = value;
							newRoot.KeyCount = 1;
							newRoot.Pointer[0][1] = n.Address;
							newRoot.Pointer[1][1] = newNode.Address;
							b.Root = newRoot.Address;
						}
						newNode.Father = n.Father;
						if (n.isLeaf)
							n.Next = newNode.Address;

						n.SetFile(p);
						file.Seek(11 + (n.Address - 1) * 19*4, CFile::begin);
						file.Write(p, 19*4);
						newNode.SetFile(p);
						file.Seek(2 + (newNode.Address - 1) * 19, CFile::begin);
						file.Write(p, 19*4);

						file.Seek((n.Father - 1) * 19*4 + 11, CFile::begin);
						file.Read(p, 19*4);
						n.GetFile(p);
					}
					n.Key[n.KeyCount][0] = value;
					n.Pointer[n.KeyCount][0] = block;
					n.Pointer[n.KeyCount][1] = position;
					n.KeyCount++;
					n.SetFile(p);
					file.Seek(11 + (n.Address - 1) * 19*4, CFile::begin);
					file.Write(p, 19*4);
					return;
				}
				else if (iter->type == MYCHAR) {
					int length = iter->length;
					string value = v;
					Node<char> n(b.Root, length);
					float* p = new float[16 + 3 * length];
					file.Seek(2 + (b.Root - 1) * (16 + 3 * length)*4, CFile::begin);
					file.Read(p, (16 + 3 * length)*4);
					n.GetFile(p);
					while (!n.isLeaf) {
						for (j = 0; j < n.KeyCount; j++) {
							if (n.Key[j] == value) {
								cout << "Key exist" << endl;
								return;
							}
							if (n.Key[j] > value) break;
						}
						int offset = n.Pointer[j][1];
						file.Seek(2 + (offset - 1) * (16 + 3 * length)*4, CFile::begin);
						file.Read(p, (16 + 3 * length)*4);
						n.GetFile(p);
					}
					if (n.KeyCount < 2) {
						n.Key[n.KeyCount] = (char*)value.c_str();
						n.Pointer[n.KeyCount][0] = block;
						n.Pointer[n.KeyCount][1] = position;
						n.KeyCount++;
						n.SetFile(p);
						file.Seek(11 + (n.Address - 1) * (16 + 3 * length)*4, CFile::begin);
						file.Write(p, (16 + 3 * length)*4);
						return;
					}


					while (n.KeyCount >= 2) {
						Node<char> newNode((b.MaxOffset - 1) * (16 + 3 * length) + 2, 1);
						b.MaxOffset++;

						newNode.Key[0] = (char*)value.c_str();
						newNode.Pointer[0][0] = block;
						newNode.Pointer[0][1] = position;
						newNode.KeyCount = 1;
						newNode.isLeaf = n.isLeaf;
						if (newNode.isLeaf)
							newNode.Next = n.Next;
						if (n.Father == 0) {
							Node<char> newRoot((b.MaxOffset - 1) * (16 + 3 * length) + 2, 1);
							b.MaxOffset++;
							n.Father = newRoot.Address;
							newRoot.Key[0] = (char*)value.c_str();
							newRoot.KeyCount = 1;
							newRoot.Pointer[0][1] = n.Address;
							newRoot.Pointer[1][1] = newNode.Address;
							b.Root = newRoot.Address;
						}
						newNode.Father = n.Father;
						if (n.isLeaf)
							n.Next = newNode.Address;

						n.SetFile(p);
						file.Seek(11 + (n.Address - 1) * (16 + 3 * length)*4, CFile::begin);
						file.Write(p, (16 + 3 * length)*4);
						newNode.SetFile(p);
						file.Seek(11 + (newNode.Address - 1) * (16 + 3 * length)*4, CFile::begin);
						file.Write(p, (16 + 3 * length)*4);

						file.Seek((n.Father - 1) * (16 + 3 * length)*4 + 11, CFile::begin);
						file.Read(p, (16 + 3 * length)*4);
						n.GetFile(p);
					}
					n.Key[n.KeyCount] = (char*)value.c_str();
					n.Pointer[n.KeyCount][0] = block;
					n.Pointer[n.KeyCount][1] = position;
					n.KeyCount++;
					n.SetFile(p);
					file.Seek(11 + (n.Address - 1) * (16 + 3 * length)*4, CFile::begin);
					file.Write(p, (16 + 3 * length)*4);
					return;
				}
				int p[2];
				p[0] = b.Root;
				p[1] = b.MaxOffset;
				file.Seek(4, CFile::begin);
				file.Write(p, 8);
				file.Close();
			}
		}
	}
}

void Index::DeleteNode(Table & table, const vector<Attribute>& attributes, const vector<Condition>& conditions)
{
	int selectAttriNum;
	vector<Attribute> selectAttribute;
	if (attributes[0].name == "*")
	{
		selectAttriNum = table.attributes.size();
		selectAttribute = table.attributes;
	}
	else
	{
		selectAttriNum = attributes.size();
		selectAttribute = attributes;
	}
	vector<string> * cluster = new vector<string>[selectAttriNum];
	string * allAttrValuestemp = new string[table.attriNum];

	string bot = "", top = "";
	for (int i = 0; i < selectAttriNum; i++) {
		if (conditions[i].relationType == GREATER || conditions[i].relationType == GREATER_EQUAL) {
			if (bot == "") bot = conditions[i].value;
			else if (conditions[i].value > bot)bot = conditions[i].value;
		}
		if (conditions[i].relationType == SMALLER || conditions[i].relationType == SMALLER_EQUAL) {
			if (top == "")top = conditions[i].value;
			else if (conditions[i].value < top) top = conditions[i].value;
		}
	}
	string indexName;
	int i;
	for (i = 0; i < selectAttriNum; i++) {
		if (attributes[i].indexName == conditions[i].attribute.indexName) {
			if (attributes[i].indexName == "")continue;
			indexName = attributes[i].name;
			break;
		}
	}


	CFile file;
	indexName = "./im/" + indexName;
	file.Open(indexName.c_str(), CFile::modeReadWrite);
	int f[3];
	file.Read(f, 12);
	BTree b;
	b.inUse = f[0];
	b.Root = f[1];
	b.MaxOffset = f[2];

	if (attributes[i].type == MYINT) {
		int value = atoi(bot.c_str());
		Node<int> n(b.Root, 1);
		int p[19];
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (*n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*n.Key[j] >= value) break;
		}
		value = atoi(top.c_str());
		Node<int> m(b.Root, 1);
		file.Seek(2 + (b.Root - 1) * 19, CFile::begin);
		file.Read(p, 19);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (*m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*m.Key[j] >= value) break;
		}

		Node<int> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {
			it.deleteTag = true;
			if (it.Next == 0) break;
			file.Seek((it.Next-1)*19*4, CFile::begin);
			file.Read(p, 19*4);
			it.GetFile(p);
		}

	}
	else if (attributes[i].type == MYFLOAT) {
		float value = atof(bot.c_str());
		Node<float> n(b.Root, 1);
		int p[19];
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (*n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*n.Key[j] >= value) break;
		}
		value = atof(top.c_str());
		Node<float> m(b.Root, 1);
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (*m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*m.Key[j] >= value) break;
		}

		Node<float> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {
			it.deleteTag = true;
			if (it.Next == 0) break;
			file.Seek((it.Next-1)*19*4, CFile::begin);
			file.Read(p, 19*4);
			it.GetFile(p);
		}
	}
	else if (attributes[i].type == MYCHAR) {
		int length = attributes[i].length;
		string value = bot;
		Node<char> n(b.Root, length);
		float* p = new float[3 * length + 16];
		file.Seek(2 + (b.Root - 1) * (3 * length + 16)*4, CFile::begin);
		file.Read(p, (3 * length + 16)*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(2 + (offset - 1) * (3 * length + 16)*4, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (n.Key[j] >= value) break;
		}
		value = atoi(top.c_str());
		Node<char> m(b.Root, 1);
		file.Seek(2 + (b.Root - 1) * (3 * length + 16)*4, CFile::begin);
		file.Read(p, (3 * length + 16)*4);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(2 + (offset - 1) * (3 * length + 16)*4, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (m.Key[j] >= value) break;
		}

		Node<char> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {
			it.deleteTag = true;
			if (it.Next == 0) break;
			file.Seek((it.Next-1)*(3 * length + 16) *4, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			it.GetFile(p);
		}
	}
}

void Index::DropIndex(string IndexName)
{
	CFile file;
	IndexName = "./im/" + IndexName;
	const char* s = IndexName.c_str();
	bool open = file.Open(s, CFile::modeReadWrite);
	if (!open) {
		cout << "Cannot drop index!" << endl;
		return;
	}
	float* temp;
	file.Read(temp, 4);
	if (*temp != 1) cout << "Index not Exist" << endl;
	else {
		temp[0] = 0;
		file.Write(temp, 4);
		cout << "index " << IndexName << " drop success" << endl;
	}
}

void Index::DropAll(Table & table)
{
	vector<Attribute>::iterator iter;
	for (iter = table.attributes.begin(); iter != table.attributes.end(); iter++)
	{
		if (iter->indexName != "")
		{
			string indexName = "./im/" + iter->indexName;
			const char* s = indexName.c_str();
			DeleteFileA(s);
		}
	}
}

void Index::FindNode(Table & table, const vector<Attribute>& attributes, const vector<Condition>& conditions)
{
	int selectAttriNum;
	vector<Attribute> selectAttribute;
		selectAttriNum = table.attributes.size();
		selectAttribute = table.attributes;
	vector<string> * cluster = new vector<string>[selectAttriNum];
	string * allAttrValuestemp = new string[table.attriNum];

	string bot = "", top = "";
	for (int i = 0; i < selectAttriNum; i++) {
		if (conditions[i].relationType == GREATER || conditions[i].relationType == GREATER_EQUAL) {
			if (bot == "") bot = conditions[i].value;
			else if (conditions[i].value > bot)bot = conditions[i].value;
		}
		if (conditions[i].relationType == SMALLER || conditions[i].relationType == SMALLER_EQUAL) {
			if (top == "")top = conditions[i].value;
			else if (conditions[i].value < top) top = conditions[i].value;
		}
	}
	string indexName;
	int i;
	for (i = 0; i < selectAttriNum; i++) {
		if (attributes[i].indexName == conditions[i].attribute.indexName) {
			if (attributes[i].indexName == "")continue;
			indexName = attributes[i].name;
			break;
		}
	}


	CFile file;
	indexName = "./im/" + indexName;
	file.Open(indexName.c_str(), CFile::modeReadWrite);
	float f[3];
	file.Read(f, 12);
	BTree b;
	b.inUse = f[0];
	b.Root = f[1];
	b.MaxOffset = f[2];

	if (attributes[i].type == MYINT) {
		int value = atoi(bot.c_str());
		Node<int> n(b.Root, 1);
		float p[19];
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (*n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*n.Key[j] >= value) break;
		}
		value = atoi(top.c_str());
		Node<int> m(b.Root, 1);
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (*m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*m.Key[j] >= value) break;
		}

		Node<int> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {

			for (int i = 0; i < it.KeyCount; i++) {
				if (!it.deleteTag) {
					string v = rm.GetStringValue(bm.BufferBlock[it.Pointer[i][0]].value + it.Pointer[i][0] + it.Pointer[i][1], table.attributes[i].length, table.attributes[i].type);
					if (rm.contains(selectAttribute, table.attributes[i]))
					{
						cluster[count].push_back(v);
						count++;
					}
				}
			}
			for (int i = 0; i < selectAttriNum; i++)
			{
				attri.insert(std::pair<string, vector<string> >(selectAttribute[i].name, cluster[i]));
			}
			if (it.Next == 0) break;
			file.Seek((it.Next-1)*4*19+11, CFile::begin);
			file.Read(p, 19*4);
			it.GetFile(p);
		}

		for (auto& result : attri) {
			cout << setw(20) << setiosflags(ios::left) << result.first;
		}
		cout << endl;
		for (int i = 0; i < cluster[0].size; i++)
		{
			for (auto& result : attri) {
				cout << setw(20) << setiosflags(ios::left) << result.second[i];
			}
			cout << endl;
		}
	}
	else if (attributes[i].type == MYFLOAT) {
		float value = atof(bot.c_str());
		Node<float> n(b.Root, 1);
		float p[19];
		file.Seek(2 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (*n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*n.Key[j] >= value) break;
		}
		value = atof(top.c_str());
		Node<float> m(b.Root, 1);
		file.Seek(11 + (b.Root - 1) * 19*4, CFile::begin);
		file.Read(p, 19*4);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (*m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(11 + (offset - 1) * 19*4, CFile::begin);
			file.Read(p, 19*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (*m.Key[j] >= value) break;
		}

		Node<float> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {
			for (int i = 0; i < it.KeyCount; i++) {
				if (!it.deleteTag) {
					string v = rm.GetStringValue(bm.BufferBlock[it.Pointer[i][0]].value + it.Pointer[i][0] + it.Pointer[i][1], table.attributes[i].length, table.attributes[i].type);
					if (rm.contains(selectAttribute, table.attributes[i]))
					{
						cluster[count].push_back(v);
						count++;
					}
				}
			}
			for (int i = 0; i < selectAttriNum; i++)
			{
				attri.insert(std::pair<string, vector<string> >(selectAttribute[i].name, cluster[i]));
			}
			if (it.Next == 0) break;
			file.Seek((it.Next-1)*4*19, CFile::begin);
			file.Read(p, 19*4);
			it.GetFile(p);
		}

		for (auto& result : attri) {
			cout << setw(20) << setiosflags(ios::left) << result.first;
		}
		cout << endl;
		for (int i = 0; i < cluster[0].size; i++)
		{
			for (auto& result : attri) {
				cout << setw(20) << setiosflags(ios::left) << result.second[i];
			}
			cout << endl;
		}
	}
	else if (attributes[i].type == MYCHAR) {
		int length = attributes[i].length;
		string value = bot;
		Node<char> n(b.Root, length);
		float* p = new float[3 * length + 16];
		file.Seek(2 + (b.Root - 1) * (3 * length + 16)*4, CFile::begin);
		file.Read(p, (3 * length + 16)*4);
		n.GetFile(p);
		int j;
		while (!n.isLeaf) {
			for (j = 0; j < n.KeyCount; j++) {
				if (n.Key[j] >= value) break;
			}
			int offset = n.Pointer[j][1];
			file.Seek(2 + (offset - 1) * (3 * length + 16)*4, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			n.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (n.Key[j] >= value) break;
		}
		value = atoi(top.c_str());
		Node<char> m(b.Root, 1);
		file.Seek(2 + (b.Root - 1) * (3 * length + 16)*4, CFile::begin);
		file.Read(p, (3 * length + 16)*4);
		m.GetFile(p);
		int j;
		while (!m.isLeaf) {
			for (j = 0; j < m.KeyCount; j++) {
				if (m.Key[j] >= value) break;
			}
			int offset = m.Pointer[j][1];
			file.Seek(2 + (offset - 1) * (3 * length + 16)*4, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			m.GetFile(p);
		}
		for (j = 0; j < n.KeyCount; j++) {
			if (m.Key[j] >= value) break;
		}

		Node<char> it(1, 1);
		int count = 0;
		for (it = n; it.Address != m.Address;) {
			for (int i = 0; i < it.KeyCount; i++) {
				if (!it.deleteTag) {
					string v = rm.GetStringValue(bm.BufferBlock[it.Pointer[i][0]].value + it.Pointer[i][0] + it.Pointer[i][1], table.attributes[i].length, table.attributes[i].type);
					if (rm.contains(selectAttribute, table.attributes[i]))
					{
						cluster[count].push_back(v);
						count++;
					}
				}
			}
			for (int i = 0; i < selectAttriNum; i++)
			{
				attri.insert(std::pair<string, vector<string> >(selectAttribute[i].name, cluster[i]));
			}
			if (it.Next == 0) break;
			file.Seek(it.Next, CFile::begin);
			file.Read(p, (3 * length + 16)*4);
			it.GetFile(p);
		}

		for (auto& result : attri) {
			cout << setw(20) << setiosflags(ios::left) << result.first;
		}
		cout << endl;
		for (int i = 0; i < cluster[0].size; i++)
		{
			for (auto& result : attri) {
				cout << setw(20) << setiosflags(ios::left) << result.second[i];
			}
			cout << endl;
		}
	}


}
