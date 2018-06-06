#ifndef _SELECT_H_
#define _SELECT_H_

#include "Table.h"
#include <vector>

using namespace std;

class Select :public Table
{
private:
	int display_row_num;
	bool is_display_row[COLUMNMAXSIZE];
	int condition_num;
	vector<string> condition_name;
	vector<string> condition_op;
	vector<string> condition_value;
public:
	Select();
	~Select();

	int getDisplayRowNum();
	bool getIsDisplayRow();
	int getConditionNum();
	string getConditionName(int);
	string getConditionOp(int);
	string getConditionValue(int);
	
};

#endif // !_SELECT_H_

