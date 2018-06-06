#ifndef _OPERATION_H_
#define _OPERATION_H_
/*
 * operation_type: 
 *     01.create table  ����;
 *     02.drop   table  ����;
 *     10.create index  ����;
 *     11.drop   index  ����;
 *     20.select        ���� ��where; 
 *     21.select        ���� ��where;
 *     30.insert        ����;
 *     40.delete        ���� ��where;
 *	   41.delete        ���� ��where��
 *     50.execfile      ����;
 *     60.quit          ����;
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

	//create table������غ���������
	string execute(CreateTable&);
	string catalogExecute(CreateTable&);
	string indexExecute(CreateTable&);

	//drop table������غ���������
	string execute(DropTable&);
	string catalogExecute(DropTable&);
	string recordExecute(DropTable&);
	string indexExecute(DropTable&);

	//insert������غ���������
	string execute(Insert&);
	string catalogExecute(Insert&);
	string recordExecute(Insert&);
	string indexExecute(Insert&);

	//delete������غ���
	string execute(Delete&);
	string catalogExecute(Delete&);
	string recordExecute(Delete&);
	string indexExecute(Delete&);

	//create index������غ����Ĳ���
	string execute(CreateIndex&);
	string catalogExecute(CreateIndex&);
	string indexExecute(CreateIndex&);

	//drop index������غ���
	string execute(DeleteIndex&);
	string catalogExecute(DeleteIndex&);
	string indexExecute(DeleteIndex&);

	//select index������غ���
	string execute(Select&);
	string catalogExecute(Select&);
	string recordExecute(Select&);


};

#endif
