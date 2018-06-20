#ifndef _INPUTPROCESS_H_
#define _INPUTPROCESS_H_

/*
* operation_type:
*     01.create table  operation
*     02.drop   table  operation
*     10.create index  operation
*     11.drop   index  operation
*     20.select        operation no where
*     21.select        operation where
*     30.insert        operation
*     40.delete        operation no where
*	  41.delete        operation where
*     50.execfile      operation
*     60.quit          operation
*/

using std::string;

void logInterface();

void execInterface();

int sqlConvert(string &);

int createConvert(string &);
int createIndexConvert(string &);
int createTableConvert(string &);
int createTableConditionConvert(string &, CreateTable &);
int createTablePrimaryConvert(string &, CreateTable &);

int deleteConvert(string &);
int deleteConditionConvert(string &, Delete &);

int dropConvert(string &);
int dropTableConvert(string &);
int dropIndexConvert(string &);

int insertConvert(string &);
int insertConditionConvert(string &, Insert &);

int selectConvert(string &);
int selectConditionConvert(string &, Select &);

int execfileConvert(string &);
void getFileConvert(string &);

bool isSqlEnd(string &);

void removeSpace(string &);
void removeSpaceAll(string &);
void outputError(int opt);


#endif // !_INPUTPROCESS_H_


