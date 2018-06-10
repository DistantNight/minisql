#ifndef _OPERATIONINPUT_H_
#define _OPERATIONINPUT_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std; // TODO: Do NOT using namespace in head file
#define COLUMNMAXSIZE 32 

typedef enum COLUMNTYPE
{
	INT, CHAR, FLOAT
}ColumnType;

/*
* name : Table
* @para :
*   table_name : name of table
*   column_num : the number of table's attributes
*   column_type[] : ColumnType array to store the type of data
*   string_length[] : int array to store the max length of the char
*   is_unique : bool array to store the check of attributes
*	 primary_key : the the name of the primary key
*/
class Table
{
public:
	string table_name;
	int column_num;
	string column_name[COLUMNMAXSIZE];
	ColumnType column_type[COLUMNMAXSIZE];
	int string_length[COLUMNMAXSIZE];
	bool is_unique[COLUMNMAXSIZE];
	string primary_key;
	int exist_index_num;
	vector<string> exist_index_name;
public:
	//Table();
	//~Table();
};

/*
* name : CreateTable
* @para :
*	 primary_key : the the name of the primary key
*/
class CreateTable :public Table
{
public:
	string primary_key;
public:
	CreateTable();
	~CreateTable();
};

/*
* name : DropTable
* @para :
*/
class DropTable
{
public:
	DropTable();
	~DropTable();
};

/*
* name : CreateIndex
* @para :
*	 index_name : the name of index we need to create
*    index_column_name : the name of attributes used to create index
*/
class CreateIndex :public Table
{
public:
	string index_name;
	string index_column_name;
public:
	CreateIndex();
	~CreateIndex();

};

/*
* name : DropIndex
* @para :
*	 index_name : the name of index we need to drop
*/
class DropIndex :public Table
{
public:
	string index_name;
public:
	DropIndex();
	~DropIndex();

};

/*
* name : Insert
* @para :
*	 row_value : string array to store the data
*    value_num : the number of the insert data
*/
class Insert :public Table
{
public:
	string row_value[COLUMNMAXSIZE];
	int value_num;
public:
	//Insert();
	//~Insert();
};

/*
* name : Delete
* @para :
*	 condition_num : the num of the condition
*	 condition_name : the name of attributes
*    condition_op : the operator =	<>	<	>	<=	>=
*	 condition_value : the value need to compare
*/
class Delete :public Table
{
public:
	int condition_num;
	vector<string> condition_name;
	vector<string> condition_op;
	vector<string> condition_value;
public:
	//Delete();
	//~Delete();

};

/*
* name : Select
* @para :
*	 condition_num : the num of the condition
*	 condition_name : the name of attributes
*    condition_op : the operator =	<>	<	>	<=	>=
*	 condition_value : the value need to compare
*/
class Select :public Table
{
public:
	int condition_num;
	vector<string> condition_name;
	vector<string> condition_op;
	vector<string> condition_value;
public:
	//Select();
	//~Select();
};


#endif // !_OPERATIONINPUT_H_

