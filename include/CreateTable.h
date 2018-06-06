#ifndef _CREATETABLE_H_
#define _CREATETABLE_H_

#include <iostream>
#include "Table.h"

using namespace std;

class CreateTable:public Table
{
private:
	string primary_key;
public:
	CreateTable();
	~CreateTable();
	
	string getPrimaryKey();
	
};

#endif

