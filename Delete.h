#ifndef _DELETE_H_
#define _DELETE_H_

#include "Table.h"
#include <vector>
#include <string>

using namespace std;

class Delete:public Table
{
private:
	int condition_num;
	vector<string> condition_name;
	vector<string> condition_op;
	vector<string> condition_value;
public:
	Delete();
	~Delete();

	int getConditionNum();
	string getConditionName(int);
	string getConditionOp(int);
	string getConditionValue(int);

};

#endif // !_DELETE_H_

