#ifndef _INDEXOPERATION_H_
#define _INDEXOPERATION_H_

#include "Table.h"
#include <iostream>
#include <string>

using namespace std;

class CreateIndex :public Table
{
private:
	string index_name;
	string index_column_name;
public:
	CreateIndex();
	~CreateIndex();

	string getIndexName();
	string getIndexColumnName();

};

class DeleteIndex :public Table
{
private:
	string index_name;
public:
	DeleteIndex();
	~DeleteIndex();

	string getIndexName();

};



#endif // !_CREATEINDEX_H_
