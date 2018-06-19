#include "API.h"
#include "catalog.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
class catalogManager;
extern catalogManager x;

bool getTableInfo(Table & a) {

	x.myprint(a.table_name);
    return true;
}

string catalogExecute(CreateTable & b) {
	vector<string> colname;
	for (unsigned long long i = 0; i < b.column_name->length(); i++) {
		colname.push_back(b.column_name[i]);
	}
	vector<string> coltype;
	for (int i = 0; i < b.column_num; i++) {
		//coltype.push_back(b.column_type[i]);
		if (b.column_type[i] == INT) {
			coltype.emplace_back("int");
		}
		else if (b.column_type[i] == CHAR) {
			coltype.emplace_back("char");
		}
		else if (b.column_type[i] == FLOAT) {
			coltype.emplace_back("float");
		}
	}
	vector<int> isU;
	for (int i = 0; i < b.column_num; i++) {
		isU[i] = static_cast<int>(b.is_unique[i]);
	}
	bool j = x.createTable(b.table_name, colname, coltype, isU, b.primary_key);
	if (j == 0) {
		cout << "create fail" << endl;
		return 0;
	}
	cout << "creatr success" << endl;
	//x.updateCatalog();
	return 0;
}

string catalogExecute(DropTable & c) {
	int judge = x.isTableExist(c.table_name);
	if (judge == 0) {
		cout << "no table" << endl;
		return 0;
	}
	x.dropTable(c.table_name);
	cout << "drop success" << endl;
	return 0;
}
bool catalogExecute(CreateIndex &d) {
	int judge = x.isIndexExist(d.index_name);
	if (judge == 1) {
		cout << "index exist" << endl;
		return 0;
	}
	x.createIndex(d.index_name, d.table_name, d.index_column_name);
	cout << "create index successfully" << endl;
	return 1;

}
bool catalogExecute(DropIndex &e) {
	int judge = x.isIndexExist(e.index_name);
	if (judge = 0) {
		cout << "index not exist" << endl;
		return 0;
	}
	x.deleteIndex(e.index_name);
	cout << "delete successfully" << endl;
	return 1;

}
bool catalogExecute(Insert &f) {
	int j = x.isTableExist(f.table_name);
	if (j == 0) {
		cout << "insert error" << endl;
		return 0;
	}
	return x.numberOfRecordAdd(f.table_name, 1);
}
bool catalogExecute(Delete &g) {
	int a = x.isTableExist(g.table_name);
	if (a == 0) {
		cout << "no table" << endl;
		return 0;
	}
	if (x.deleteTableInfo(g.table_name, 1)) {
		cout << "delete success" << endl;
		return 1;
	}
	else {
		cout << "fail to delete" << endl;
		return 0;
	}
}
bool catalogExecute(Select &h) {
	int a = x.isTableExist(h.table_name);
	if (a == 0) {
		cout << "table not exist" << endl;
		return 0;
	}
	int flag = 0;
	vector<string> s = x.nameOfKey(h.table_name);
	if (h.condition_name[0] == "*") {
		cout << "select success" << endl;
		return 1;
	}
	for (int i = 0; i < s.size(); i++) {
		if (h.condition_name[0] == s[i]) flag = 1;
	}
	if (flag == 1) {
		cout << "select success" << endl;
		return 1;
	}
	else {
		cout << "select error" << endl;
		return 0;
	}
}