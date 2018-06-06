#ifndef _TABLE_H_
#define _TABLE_H_

#include <iostream>
#include <string>
//题目声明的表最多包含属性值
#define COLUMNMAXSIZE 32 
typedef enum COLUMNTYPE
{
	INT, CHAR, FLOAT
} ColumnType;

using namespace std;

class Table
{
private:
	static string table_name;
	static int column_num;
	static string column_name[COLUMNMAXSIZE];
	static ColumnType column_type[COLUMNMAXSIZE];
	static int string_length[COLUMNMAXSIZE];
	static bool is_unique[COLUMNMAXSIZE];
public:
	Table();
	~Table();

	string getTableName();//返回表名
	int getColumnNum();//返回表总列数
	ColumnType getColumnType(int);//返回每个属性的类型
	int getStringLength(int);//返回字符串的最大长度
	bool getIsUnique(int);//返回该项是否为unique
};

#endif
