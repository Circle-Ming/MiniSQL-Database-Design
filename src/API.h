#ifndef API_H_
#define API_H_
#include<afx.h>
#include "MiniSQL.h"

class API
{
public:
	API() {};
	~API() {};
	void APIM(SQLstatement &s);
	void APIL(SQLstatement &s);
	string read();
	string down(string s);
};

#endif
