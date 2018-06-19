#ifndef _API_H_
#define _API_H_

using std::string;

// 从catalog management中获取表的所有信息，table name已实现储存在输入中
bool getTableInfo(Table &);
// 执行create table，最后catalog management返回格式化结果输出字符串
string catalogExecute(CreateTable &);
//bool indexExecute(CreateTable &);

// 执行drop table，最后catalog management返回格式化结果输出字符串
string catalogExecute(DropTable &);
bool recordExecute(DropTable &);
bool indexExecute(DropTable &);

// 执行create index，最后index management返回格式化结果输出字符串
bool catalogExecute(CreateIndex &);
string indexExecute(CreateIndex &);

// 执行drop index，最后index management返回格式化结果输出字符串
bool catalogExecute(DropIndex &);
string indexExecute(DropIndex &);

// 执行insert，最后record management返回格式化结果输出字符串
bool catalogExecute(Insert &);
string recordExecute(Insert &);
//bool indexExecute(Insert &);

// 执行delete，最后record management返回格式化结果输出字符串
bool catalogExecute(Delete &);
string recordExecute(Delete &);
//bool indexExecute(Delete &);

// 执行select，最后record management返回格式化结果输出字符串
bool catalogExecute(Select &);
string recordExecute(Select &);



#endif
