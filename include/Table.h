#ifndef _TABLE_H_
#define _TABLE_H_

#include <iostream>
#include <string>
//��Ŀ�����ı�����������ֵ
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

	string getTableName();//���ر���
	int getColumnNum();//���ر�������
	ColumnType getColumnType(int);//����ÿ�����Ե�����
	int getStringLength(int);//�����ַ�������󳤶�
	bool getIsUnique(int);//���ظ����Ƿ�Ϊunique
};

#endif
