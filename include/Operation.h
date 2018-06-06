#ifndef _OPERATION_H_
#define _OPERATION_H_
/*
 * operation_type: 
 *     01.create table  操作;
 *     02.drop   table  操作;
 *     10.create index  操作;
 *     11.drop   index  操作;
 *     20.select        操作 无where; 
 *     21.select        操作 有where;
 *     30.insert        操作;
 *     40.delete        操作 无where;
 *	   41.delete        操作 有where；
 *     50.execfile      操作;
 *     60.quit          操作;
 */

#include <iostream>
#include <string>
#include "Table.h"
#include "CreateTable.h"
#include "DropTable.h"
#include "Insert.h"
#include "Delete.h"
#include "IndexOperation.h"
#include "Select.h"

using namespace std;

class Operation
{
private:
	static int operation_type;
public:
	Operation();
	~Operation();

	int getType();
	void setType(int);
	bool isError();
	bool getTableInfo(Table&);

	//create table操作相关函数的重载
	string execute(CreateTable&);
	string catalogExecute(CreateTable&);
	string indexExecute(CreateTable&);

	//drop table操作相关函数的重载
	string execute(DropTable&);
	string catalogExecute(DropTable&);
	string recordExecute(DropTable&);
	string indexExecute(DropTable&);

	//insert操作相关函数的重载
	string execute(Insert&);
	string catalogExecute(Insert&);
	string recordExecute(Insert&);
	string indexExecute(Insert&);

	//delete操作相关函数
	string execute(Delete&);
	string catalogExecute(Delete&);
	string recordExecute(Delete&);
	string indexExecute(Delete&);

	//create index操作相关函数的操作
	string execute(CreateIndex&);
	string catalogExecute(CreateIndex&);
	string indexExecute(CreateIndex&);

	//drop index操作相关函数
	string execute(DeleteIndex&);
	string catalogExecute(DeleteIndex&);
	string indexExecute(DeleteIndex&);

	//select index操作相关函数
	string execute(Select&);
	string catalogExecute(Select&);
	string recordExecute(Select&);


};

#endif
