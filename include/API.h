#ifndef _API_H_
#define _API_H_

using std::string;

// ��catalog management�л�ȡ���������Ϣ��table name��ʵ�ִ�����������
bool getTableInfo(Table &);
// ִ��create table�����catalog management���ظ�ʽ���������ַ���
string catalogExecute(CreateTable &);
//bool indexExecute(CreateTable &);

// ִ��drop table�����catalog management���ظ�ʽ���������ַ���
string catalogExecute(DropTable &);
bool recordExecute(DropTable &);
bool indexExecute(DropTable &);

// ִ��create index�����index management���ظ�ʽ���������ַ���
bool catalogExecute(CreateIndex &);
string indexExecute(CreateIndex &);

// ִ��drop index�����index management���ظ�ʽ���������ַ���
bool catalogExecute(DropIndex &);
string indexExecute(DropIndex &);

// ִ��insert�����record management���ظ�ʽ���������ַ���
bool catalogExecute(Insert &);
string recordExecute(Insert &);
//bool indexExecute(Insert &);

// ִ��delete�����record management���ظ�ʽ���������ַ���
bool catalogExecute(Delete &);
string recordExecute(Delete &);
//bool indexExecute(Delete &);

// ִ��select�����record management���ظ�ʽ���������ַ���
bool catalogExecute(Select &);
string recordExecute(Select &);



#endif
