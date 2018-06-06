#ifndef _INSERT_H_
#define _INSERT_H_

#include "Table.h"

class Insert :public Table
{
private:
	string row_value[COLUMNMAXSIZE];
	int value_num;
public:
	Insert();
	~Insert();

	int getValueNum();
	string getRowValue(int);
};

#endif // ! _INSERT_H_

