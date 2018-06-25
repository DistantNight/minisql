#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ostream>
#include "../include/OperationInput.h"
#include "../include/API.h"
#include "../include/InputProcess.h"

using namespace std;

string root_name("root");
string root_password("456123asd");

string lag_create("create");
string lag_drop("drop");
string lag_delete("delete");
string lag_select("select");
string lag_insert("insert");
string lag_quit("quit");
string lag_execfile("execfile");
string lag_index("index");
string lag_table("table");
string lag_on("on");
string lag_int("int");
string lag_float("float");
string lag_char("char");
string lag_unique("unique");
string lag_primary("primary");
string lag_key("key");
string lag_from("from");
string lag_where("where");
string lag_and("and");
string lag_into("into");
string lag_values("values");
string lag_op_1("=");
string lag_op_2("<>");
string lag_op_3("<");
string lag_op_4(">");
string lag_op_5("<=");
string lag_op_6(">=");

//输入账户密码的登录界面
void logInterface()
{
	bool isLog = false;
	string user_name, user_password;

	while (!isLog)
	{
		cout << "Enter the user name: ";
		getline(cin, user_name);
		cout << "Enter the password: ";
		getline(cin, user_password);
		if (user_name == root_name && user_password == root_password)
		{
			isLog = true;
		}
		else
		{
            cout << "Wrong user name or password" << endl;
		}
	}
	cout << "Welcome to the MiniSQL. All commands end with ';'" << endl;

}

//输入执行语句的功能界面
void execInterface()
{
    bool isQuit = false;
	string temp;
    const string space(" ");

	while (!isQuit)
	{
		string sql_string;
		cout << "\nMiniSQL> ";
		getline(cin, sql_string);
		while (!isSqlEnd(sql_string))
		{
			cout << "      -> ";
			getline(cin, temp);
			sql_string += space;
			sql_string += temp;
		}
	    const int operation_type = sqlConvert(sql_string);
		if (operation_type == 60)
		{
			isQuit = true;
			cout << "You quit the MiniSQL." << endl;
		}
	}
}

//对输入sql语句的翻译模块
int sqlConvert(string &sql)
{
    int opt;
	string temp;

	removeSpace(sql);
    const auto pos_space = sql.find(' ');
    const auto pos_end = sql.find(';');
	if (pos_space == string::npos)
	{
		temp = sql.substr(0, pos_end);
		if (temp == lag_quit)
		{
			opt = 60;
		}
		else
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	}
	else
	{
		temp = sql.substr(0, pos_space);
		sql.erase(0, pos_space + 1);
		removeSpace(sql);
		if (temp == lag_create)
		{
			opt = createConvert(sql);
		}
		else if (temp == lag_delete)
		{
			opt = deleteConvert(sql);
		}
		else if (temp == lag_drop)
		{
			opt = dropConvert(sql);
		}
		else if (temp == lag_insert)
		{
			opt = insertConvert(sql);
		}
		else if (temp == lag_select)
		{
			opt = selectConvert(sql);
		}
		else if (temp == lag_execfile)
		{
			opt = execfileConvert(sql);
		}
		else if (temp == lag_quit)
		{
			if (sql[0] == ';')
			{
				opt = 60;
			}
			else
			{
				cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				return -1;
			}
		}
		else
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	}

	return opt;

}

//输入首个保留字为create的sql语句的翻译模块
int createConvert(string &sql)
{
	int opt;

    const auto pos_space = sql.find(' ');
	// int pos_end = sql.find(';');
	if (pos_space == string::npos)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}

    const string temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);
    if (temp == lag_index)
    {
        opt = createIndexConvert(sql);
    }
    else if (temp == lag_table)
    {
        opt = createTableConvert(sql);
    }
    else
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }
    return opt;
}

//对create index语句的翻译模块
int createIndexConvert(string &sql)
{
	// int opt = 10;
    int pos_space = sql.find(' ');

    if (pos_space < 0)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}

    string temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);
    CreateIndex CI;
    CI.index_name = temp;

    pos_space = sql.find(' ');
    if (pos_space < 0)
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }

    temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);

    if (temp == lag_on)
    {
        pos_space = sql.find(' ');
        int pos = sql.find('(');
        // int pos_end = sql.find(';');
        if (pos < 0)
        {
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
        }

        if (pos_space < 0)
        {
            temp = sql.substr(0, pos);
            sql.erase(0, pos + 1);
            CI.table_name = temp;
            pos = sql.find(')');
            if (pos < 0)
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }

            temp = sql.substr(0, pos);
            sql.erase(0, pos + 1);
            CI.index_column_name = temp;
            if (sql[0] == ';')
            {
                if (catalogExecute(CI))
                {
                    temp = indexExecute(CI);
                    cout << temp << endl;
                }
            }
            else
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }
        }
        else
        {
            if (pos_space < pos)
            {
                temp = sql.substr(0, pos_space);
                sql.erase(0, pos_space + 1);
                removeSpace(sql);
                CI.table_name = temp;

                if (sql[0] == '(')
                {
                    sql.erase(0, 1);
                    pos_space = sql.find(' ');
                    pos = sql.find(')');

                    if (pos < 0)
                    {
                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                        return -1;
                    }

                    if (pos_space < 0)
                    {
                        temp = sql.substr(0, pos);
                        sql.erase(0, pos + 1);
                        CI.index_column_name = temp;
                        if (sql[0] == ';')
                        {
                            if (catalogExecute(CI))
                            {
                                temp = indexExecute(CI);
                                cout << temp << endl;
                            }
                        }
                        else
                        {
                            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                            return -1;
                        }
                    }
                    else
                    {
                        if (pos_space > pos)
                        {
                            temp = sql.substr(0, pos);
                            sql.erase(0, pos + 1);
                            removeSpace(sql);
                            CI.index_column_name = temp;

                            if (sql[0] == ';')
                            {
                                if (catalogExecute(CI))
                                {
                                    temp = indexExecute(CI);
                                    cout << temp << endl;
                                }
                            }
                            else
                            {
                                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                                return -1;
                            }
                        }
                        else
                        {
                            temp = sql.substr(0, pos_space);
                            sql.erase(0, pos_space + 1);
                            removeSpace(sql);
                            CI.index_column_name = temp;

                            if (sql[0] == ')')
                            {
                                sql.erase(0, 1);
                                removeSpace(sql);
                                if (sql[0] == ';')
                                {
                                    if (catalogExecute(CI))
                                    {
                                        temp = indexExecute(CI);
                                        cout << temp << endl;
                                    }
                                }
                                else
                                {
                                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                                    return -1;
                                }
                            }
                            else
                            {
                                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                                return -1;
                            }
                        }
                    }
                }
                else
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }

            }
            else
            {
                temp = sql.substr(0, pos);
                sql.erase(0, pos + 1);
                removeSpace(sql);
                CI.table_name = temp;

                pos_space = sql.find(' ');
                pos = sql.find(')');
                if (pos < 0)
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }

                if (pos_space < 0)
                {
                    temp = sql.substr(0, pos);
                    sql.erase(0, pos + 1);
                    CI.index_column_name = temp;

                    if (sql[0] == ';')
                    {
                        if (catalogExecute(CI))
                        {
                            temp = indexExecute(CI);
                            cout << temp << endl;
                        }
                    }
                    else
                    {
                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                        return -1;
                    }
                }
                else
                {
                    if (pos_space > pos)
                    {
                        temp = sql.substr(0, pos);
                        sql.erase(0, pos + 1);
                        removeSpace(sql);
                        CI.index_column_name = temp;

                        if (sql[0] == ';')
                        {
                            if (catalogExecute(CI))
                            {
                                temp = indexExecute(CI);
                                cout << temp << endl;
                            }
                        }
                        else
                        {
                            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                            return -1;
                        }
                    }
                    else
                    {
                        temp = sql.substr(0, pos_space);
                        sql.erase(0, pos_space + 1);
                        removeSpace(sql);
                        CI.index_column_name = temp;
                        if (sql[0] == ')')
                        {
                            sql.erase(0, 1);
                            removeSpace(sql);
                            if (sql[0] == ';')
                            {
                                if (catalogExecute(CI))
                                {
                                    temp = indexExecute(CI);
                                    cout << temp << endl;
                                }

                            }
                            else
                            {
                                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                                return -1;
                            }
                        }
                        else
                        {
                            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                            return -1;
                        }
                    }
                }
            }
        }
    }
    else
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }
    return 0;
}

//对create table语句的翻译模块
int createTableConvert(string &sql)
{
	int opt; // int opt = 1;
	// int pos_end; // never used
	string temp;
	CreateTable CT;

	CT.column_num = 0;
	removeSpace(sql);
    const int pos_space = sql.find(' ');
	int pos = sql.find('(');
	if (pos < 0 || sql[0] == '(')
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
	else
	{
		if (pos_space > pos)
		{
			temp = sql.substr(0, pos);
			sql.erase(0, pos + 1);
			removeSpace(sql);
			CT.table_name = temp;

			pos = sql.find(',');
			while (pos >= 0)
			{
				temp = sql.substr(0, pos + 1);
				sql.erase(0, pos + 1);
				removeSpace(sql);
				opt = createTableConditionConvert(temp, CT);
				if (opt == -1)
				{
					return -1;
				}
				pos = sql.find(',');
			}
			//primary key
			opt = createTablePrimaryConvert(sql, CT);
			if (opt == -1)
			{
				return -1;
			}
		}
		else
		{
			temp = sql.substr(0, pos_space);
			sql.erase(0, pos_space + 1);
			removeSpace(sql);
			CT.table_name = temp;
			if (sql[0] == '(')
			{
				sql.erase(0, 1);
				pos = sql.find(',');
				while (pos >= 0)
				{
					temp = sql.substr(0, pos + 1);
					sql.erase(0, pos + 1);
					removeSpace(sql);
					removeSpace(temp);
					opt = createTableConditionConvert(temp, CT);
					if (opt == -1)
					{
						return -1;
					}
					pos = sql.find(',');
				}

				opt = createTablePrimaryConvert(sql, CT);
				if (opt == -1)
				{
					return -1;
				}
			}
			else
			{
				cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				return -1;
			}
		}
		return opt;
	}
}

//create table中输入属性的翻译模块
int createTableConditionConvert(string &sql, CreateTable &CT)
{
	int opt = 1;
	int length;
	int pos_left, pos_right;

    int pos_space = sql.find(' ');

	if (pos_space < 0)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
	else
	{
		string temp = sql.substr(0, pos_space);
		sql.erase(0, pos_space + 1);
		removeSpace(sql);
		CT.column_name[CT.column_num] = temp;
		pos_space = sql.find(' ');
		int pos_end = sql.find(',');
		if (pos_space < 0)
		{
			temp = sql.substr(0, pos_end);
			pos_left = temp.find('(');
			if (pos_left < 0)
			{
				if (temp == lag_int)
				{
					CT.column_type[CT.column_num] = INT;
					CT.is_unique[CT.column_num] = false;
					CT.column_num++;
				}
				else if (temp == lag_float)
				{
					CT.column_type[CT.column_num] = FLOAT;
					CT.is_unique[CT.column_num] = false;
					CT.column_num++;
				}
				else
				{
					cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
					return -1;
				}
			}
			else
			{
				if (temp.substr(0, pos_left) == lag_char)
				{
					CT.column_type[CT.column_num] = CHAR;
					temp.erase(0, pos_left + 1);
					pos_right = temp.find(')');
					if (pos_right < 0)
					{
						cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
						return -1;
					}
				    temp.erase(pos_right, 1);
				    length = atoi(temp.c_str());
				    if (length > 0)
				    {
				        CT.string_length[CT.column_num] = length;
				        CT.is_unique[CT.column_num] = false;
				        CT.column_num++;
				    }
				    else
				    {
				        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				        return -1;
				    }
				}
				else
				{
					cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
					return -1;
				}
			}
		}
		else
		{
			temp = sql.substr(0, pos_space);
			sql.erase(0, pos_space + 1);
			removeSpace(sql);
			pos_left = temp.find('(');
			if (pos_left < 0)
			{
				if (temp == lag_int)
				{
					CT.column_type[CT.column_num] = INT;
				}
				else if (temp == lag_float)
				{
					CT.column_type[CT.column_num] = FLOAT;
				}
				else
				{
					cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
					return -1;
				}

				if (sql[0] == ';')
				{
					CT.is_unique[CT.column_num] = false;
					CT.column_num++;
				}
				else
				{
					pos_end = sql.find(',');
					temp = sql.substr(0, pos_end);
					if (temp == lag_unique)
					{
						CT.is_unique[CT.column_num] = true;
						CT.column_num++;
					}
					else
					{
						cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
						return -1;
					}
				}
			}
			else
			{
				if (temp.substr(0, pos_left) == lag_char)
				{
					CT.column_type[CT.column_num] = CHAR;
					temp.erase(0, pos_left + 1);
					pos_right = temp.find(')');
					if (pos_right < 0)
					{
						cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
						return -1;
					}
				    temp.erase(pos_right, 1);
				    // length = atoi(temp.c_str());
				    length = stoi(temp);
				    if (length > 0)
				    {
				        CT.string_length[CT.column_num] = length;

				        if (sql[0] == ',')
				        {
				            CT.is_unique[CT.column_num] = false;
				            CT.column_num++;
				        }
				        else
				        {
				            pos_end = sql.find(',');
				            temp = sql.substr(0, pos_end);
				            if (temp == lag_unique)
				            {
				                CT.is_unique[CT.column_num] = true;
				                CT.column_num++;
				            }
				            else
				            {
				                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				                return -1;
				            }
				        }
				    }
				    else
				    {
				        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				        return -1;
				    }
				}
				else
				{
					cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
					return -1;
				}
			}
		}
	}
    return opt;
}

//create table中输入主键的翻译模块
int createTablePrimaryConvert(string &sql, CreateTable &CT)
{
	int opt = 1;
	int pos;

    const auto pos_space = sql.find(' ');
	if (pos_space == string::npos)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
    string temp = sql.substr(0, pos_space);
    removeSpace(sql);
    if (temp == lag_primary)
    {
        sql.erase(0, pos_space + 1);
        removeSpaceAll(sql);
        pos = sql.find('(');
        if (pos >= 0)
        {
            temp = sql.substr(0, pos);
            sql.erase(0, pos + 1);
            if (temp == lag_key)
            {
                pos = sql.find(')');
                if (pos < 0)
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }
                temp = sql.substr(0, pos);
                sql.erase(0, pos + 1);
                if (sql[0] == ')' && sql[1] == ';')
                {
                    CT.primary_key = temp;
                    cout << catalogExecute(CT) << endl;
                }
                else
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }
            }
            else
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }
        }
        else
        {
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
        }

    }
    else
    {
        pos = sql.find(';');
        while (sql[pos] != ')')
        {
            sql.erase(pos, 1);
            pos--;
        }
        sql.erase(pos, 1);
        sql.append(1, ',');
        opt = createTableConditionConvert(sql, CT);
        if (opt == -1)
        {
            return -1;
        }
        cout << catalogExecute(CT) << endl;
    }

    return opt;
}

//输入首个保留字为delete的sql语句的翻译模块
int deleteConvert(string &sql)
{
	int opt = 40;
	// int pos;
    Delete D;

	D.condition_num = 0;
	auto pos_space = sql.find(' ');
	if (pos_space == string::npos)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
    string temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);

    if (temp == lag_from)
    {
        pos_space = sql.find(' ');
        const auto pos_end = sql.find(';');
        if (pos_space == string::npos)
        {
            if (sql[0] == ';')
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }
            temp = sql.substr(0, pos_end);
            D.table_name = temp;

            temp = recordExecute(D);
            cout << temp << endl;
        }
        else
        {
            temp = sql.substr(0, pos_space);
            sql.erase(0, pos_space + 1);
            removeSpace(sql);

            D.table_name = temp;
            getTableInfo(D);
            if (sql[0] == ';')
            {

                temp = recordExecute(D);
                cout << temp << endl;
            }
            else
            {
                pos_space = sql.find(' ');
                if (pos_space == string::npos)
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }
                temp = sql.substr(0, pos_space);
                sql.erase(0, pos_space + 1);
                removeSpace(sql);
                if (temp == lag_where)
                {
                    opt = deleteConditionConvert(sql, D);
                    if (opt == -1)
                    {
                        return -1;
                    }

                    temp = recordExecute(D);
                    cout << temp << endl;
                }
                else
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }
            }
        }
    }
    else
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }

    return opt;
}

//delete中输入条件的翻译模块
int deleteConditionConvert(string &sql, Delete &D)
{
	int opt = 40;
	int pos;

    while (true)
	{
		int pos_space = sql.find(' ');
		if (pos_space < 0)
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
		else
		{
			string temp = sql.substr(0, pos_space);
			sql.erase(0, pos_space + 1);
			removeSpace(sql);
			D.condition_name.push_back(temp);
			pos_space = sql.find(' ');
			if (pos_space < 0)
			{
				cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				return -1;
			}
			else
			{
				temp = sql.substr(0, pos_space);
				sql.erase(0, pos_space + 1);
				removeSpace(sql);
				if (temp == lag_op_1 || temp == lag_op_2 || temp == lag_op_3 || temp == lag_op_4 || temp == lag_op_5 || temp == lag_op_6)
				{
					D.condition_op.push_back(temp);
					pos_space = sql.find(' ');
					int pos_end = sql.find(';');
					if (pos_space < 0)
					{
						temp = sql.substr(0, pos_end);
						if (temp[0] != '\'')
						{
							D.condition_value.push_back(temp);
							D.condition_num++;
							break;
						}
						else
						{
							temp.erase(0, 1);
							pos = temp.find('\'');
							if (pos < 0)
							{
								cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
								return -1;
							}
							else
							{
                                temp = temp.substr(0, pos_end);
								D.condition_value.push_back(temp);
								D.condition_num++;
								break;
							}
						}
					}
					else
					{

						if (sql[0] != '\'')
						{
                            temp = sql.substr(0, pos_space);
                            sql.erase(0, pos_space + 1);
                            removeSpace(sql);
						    D.condition_value.push_back(temp);
							D.condition_num++;
							if (sql[0] == ';')
							{
								break;
							}
							else
							{
								pos_space = sql.find(' ');
								if (pos_space < 0)
								{
									cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
									return -1;
								}
								else
								{
									temp = sql.substr(0, pos_space);
									sql.erase(0, pos_space + 1);
									removeSpace(sql);
									if (temp == lag_and)
									{

									}
									else
									{
										cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
										return -1;
									}
								}
							}
						}
						else
						{
						    sql.erase(0, 1);
							pos = sql.find('\'');
							if (pos < 0)
							{
								cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
								return -1;
							}
							else
							{
                                temp = sql.substr(0, pos);
                                sql.erase(0, pos + 1);
                                removeSpace(sql);
								D.condition_value.push_back(temp);
								D.condition_num++;
								if (sql[0] == ';')
								{
									break;
								}
								else
								{
									pos_space = sql.find(' ');
									if (pos_space < 0)
									{
										cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
										return -1;
									}
									else
									{
										temp = sql.substr(0, pos_space);
										sql.erase(0, pos_space + 1);
										removeSpace(sql);
										if (temp == lag_and)
										{

										}
										else
										{
											cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
											return -1;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
					return -1;
				}
			}
		}
	}


	return opt;
}

//输入首个保留字为drop的sql语句的翻译模块
int dropConvert(string &sql)
{
	int opt = 1;

    const auto pos_space = sql.find(' ');
	// int pos_end = sql.find(';');
	if (pos_space == string::npos)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
    const string temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);
    if (temp == lag_table)
    {
        opt = dropTableConvert(sql);
    }
    else if (temp == lag_index)
    {
        opt = dropIndexConvert(sql);
    }
    else
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }

    return opt;
}

//对drop table语句的翻译模块
int dropTableConvert(string &sql)
{
    const int opt = 2;
	// int pos;
	string temp;
	DropTable DT;

	const auto pos_space = sql.find(' ');
	const auto pos_end = sql.find(';');
	if (pos_space == string::npos)
	{
		temp = sql.substr(0, pos_end);
		DT.table_name = temp;
		if (getTableInfo(DT))
		{
		    if (recordExecute(DT) && indexExecute(DT))
		    {
                cout << catalogExecute(DT) << endl;
		    }
		}
	}
	else
	{
		temp = sql.substr(0, pos_space);
		sql.erase(0, pos_space + 1);
		removeSpace(sql);
		if (sql[0] == ';')
		{
			DT.table_name = temp;
			if (getTableInfo(DT))
			{
                if (recordExecute(DT) && indexExecute(DT))
                {
                    cout << catalogExecute(DT) << endl;
                }
			}
		}
		else
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	}

	return opt;
}

//对drop index语句的翻译模块
int dropIndexConvert(string &sql)
{
	int opt = 11;
	// int pos;
	string temp;
	DropIndex DI;

	const auto pos_space = sql.find(' ');
	const auto pos_end = sql.find(';');
	if (pos_space == string::npos)
	{
		temp = sql.substr(0, pos_end);
		DI.index_name = temp;
        if (catalogExecute(DI))
        {
            temp = indexExecute(DI);
            cout << temp << endl;
        }
        else
        {
            cout << "Index has been droped successfully" << endl;
        }
	}
	else
	{
		temp = sql.substr(0, pos_space);
		sql.erase(0, pos_space + 1);
		removeSpace(sql);
		DI.index_name = temp;
		if (sql[0] != ';')
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
        if (catalogExecute(DI))
        {
            temp = indexExecute(DI);
            cout << temp << endl;
        }
        else
        {
            cout << "Index has been droped successfully" << endl;
        }
	}

	return opt;
}

//对insert语句的翻译模块
int insertConvert(string &sql)
{
	int opt = 30;
	// int pos_end;

    auto pos_space = sql.find(' ');
	if (pos_space == string::npos)
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}
    string temp = sql.substr(0, pos_space);
    sql.erase(0, pos_space + 1);
    removeSpace(sql);
    if (temp == lag_into)
    {
        pos_space = sql.find(' ');
        if (pos_space == string::npos)
        {
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
        }
        temp = sql.substr(0, pos_space);
        sql.erase(0, pos_space + 1);
        Insert I;
        I.table_name = temp;
        I.value_num = 0;
        pos_space = sql.find(' ');
        auto pos = sql.find('(');
        if (pos == string::npos)
        {
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
        }
        if(pos_space >= 0 && pos_space < pos)
        {
            temp = sql.substr(0, pos_space);
            sql.erase(0, pos_space + 1);
            removeSpace(sql);
            if(sql[0] != '(')
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }
            else
            {
                sql.erase(0, 1);
            }
        }
        else
        {
            temp = sql.substr(0, pos);
            sql.erase(0, pos + 1);
        }

        if (temp == lag_values)
        {
            pos = sql.find(')');
            temp = sql.substr(0, pos);
            sql.erase(0, pos + 1);
            removeSpace(temp);
            if (sql[0] == ';')
            {
                opt = insertConditionConvert(temp, I);
                if (opt == -1)
                {
                    return -1;
                }
                else
                {
                    if (getTableInfo(I))
                    {
                        temp = recordExecute(I);
                        cout << temp << endl;
                    }
                }
            }
            else
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }

        }
        else
        {
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
        }
    }
    else
    {
        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
        return -1;
    }

    return opt;
}

//insert中输入插入值的翻译模块
int insertConditionConvert(string &sql, Insert &I)
{
	int opt = 30;
	int pos_right, pos_space;
    string temp, temp2;

    int pos = sql.find(',');
	while (pos > 0)
	{
		temp = sql.substr(0, pos);
		sql.erase(0, pos + 1);
        removeSpace(sql);
		if (temp[0] == '\'')
		{
			temp.erase(0, 1);
			pos_right = temp.find('\'');
			if (pos_right < 0)
			{
				cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
				return -1;
			}
			else
			{
                temp = temp.substr(0, pos_right);
			}
		}
        else
        {
            pos_space = temp.find(' ');
            if(pos_space >= 0)
            {
                temp2 = temp.substr(0, pos_space);
                temp.erase(0, pos_space + 1);
                removeSpace(temp);
                if(!temp.empty())
                {
                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                    return -1;
                }
                temp = temp2;
            }
        }
		I.row_value[I.value_num] = temp;
		I.value_num++;
		pos = sql.find(',');
	}
	if (sql[0] == '\'')
	{
		sql.erase(0, 1);
		pos_right = sql.find('\'');
		if (pos_right < 0)
		{
            cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
            return -1;
		}
		else
		{
            temp = sql.substr(0, pos_right);
		}
	}
    else
    {
        pos_space = sql.find(' ');
        if(pos_space >= 0)
        {
            temp = sql.substr(0, pos_space);
            sql.erase(0, pos_space + 1);
            removeSpace(sql);
            if(!sql.empty())
            {
                cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
                return -1;
            }
        }
        else
        {
            temp = sql;
        }
    }
	I.row_value[I.value_num] = temp;
	I.value_num++;

	return opt;
}

//对select语句的翻译模块
int selectConvert(string &sql)
{
	int opt = 20;
	// int pos;
    Select S;

	S.condition_num = 0;
	removeSpace(sql);
	if (sql[0] == '*')
	{
		sql.erase(0, 1);
		removeSpace(sql);
		int pos_space = sql.find(' ');
		if (pos_space < 0)
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	    string temp = sql.substr(0, pos_space);
	    sql.erase(0, pos_space + 1);
	    removeSpace(sql);
	    if (temp == lag_from)
	    {
	        pos_space = sql.find(' ');
	        int pos_end = sql.find(';');
	        if (pos_space < 0)
	        {
	            temp = sql.substr(0, pos_end);
	            S.table_name = temp;

	            if (getTableInfo(S))
	            {
	                temp = recordExecute(S);
	                cout << temp << endl;
	            }
	        }
	        else
	        {
	            temp = sql.substr(0, pos_space);
	            sql.erase(0, pos_space + 1);
	            removeSpace(sql);
	            S.table_name = temp;
	            if (sql[0] == ';')
	            {
	                if (getTableInfo(S))
	                {
	                    temp = recordExecute(S);
	                    cout << temp << endl;
	                }

	            }
	            else
	            {
	                pos_space = sql.find(' ');
	                if (pos_space < 0)
	                {
	                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                    return -1;
	                }
	                temp = sql.substr(0, pos_space);
	                sql.erase(0, pos_space + 1);
	                removeSpace(sql);
	                if (temp == lag_where)
	                {
	                    opt = selectConditionConvert(sql, S);
	                    if (opt == -1)
	                    {
	                        return -1;
	                    }

	                    if (getTableInfo(S))
	                    {
	                        temp = recordExecute(S);
	                        cout << temp << endl;
	                    }

	                }
	                else
	                {
	                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                    return -1;
	                }
	            }
	        }
	    }
	    else
	    {
	        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	        return -1;
	    }
	}
	else
	{
		cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
		return -1;
	}

	return opt;
}

//select中输入条件的翻译模块
int selectConditionConvert(string &sql, Select &S)
{
	int opt = 20;
	int pos;

    while (true)
	{
		int pos_space = sql.find(' ');
		if (pos_space < 0)
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	    string temp = sql.substr(0, pos_space);
	    sql.erase(0, pos_space + 1);
	    removeSpace(sql);
	    S.condition_name.push_back(temp);
	    pos_space = sql.find(' ');
	    if (pos_space < 0)
	    {
	        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	        return -1;
	    }
	    temp = sql.substr(0, pos_space);
	    sql.erase(0, pos_space + 1);
	    removeSpace(sql);
	    if (temp == lag_op_1 || temp == lag_op_2 || temp == lag_op_3 || temp == lag_op_4 || temp == lag_op_5 || temp == lag_op_6)
	    {
	        S.condition_op.push_back(temp);
	        pos_space = sql.find(' ');
	        int pos_end = sql.find(';');
	        if (pos_space < 0)
	        {
	            temp = sql.substr(0, pos_end);
	            if (temp[0] != '\'')
	            {
	                S.condition_value.push_back(temp);
	                S.condition_num++;
	                break;
	            }
	            else
	            {
	                temp.erase(0, 1);
	                pos = temp.find('\'');
	                if (pos < 0)
	                {
	                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                    return -1;
	                }
	                else
	                {
	                    temp.erase(pos, 1);
	                    S.condition_value.push_back(temp);
	                    S.condition_num++;
	                    break;
	                }
	            }
	        }
	        else
	        {
	            temp = sql.substr(0, pos_space);
	            sql.erase(0, pos_space + 1);
	            removeSpace(sql);
	            if (temp[0] != '\'')
	            {
	                S.condition_value.push_back(temp);
	                S.condition_num++;
	                if (sql[0] == ';')
	                {
	                    break;
	                }
	                else
	                {
	                    pos_space = sql.find(' ');
	                    if (pos_space < 0)
	                    {
	                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                        return -1;
	                    }
	                    temp = sql.substr(0, pos_space);
	                    sql.erase(0, pos_space + 1);
	                    removeSpace(sql);
	                    if (temp == lag_and)
	                    {

	                    }
	                    else
	                    {
	                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                        return -1;
	                    }
	                }
	            }
	            else
	            {
	                temp.erase(0, 1);
	                pos = temp.find('\'');
	                if (pos < 0)
	                {
	                    cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                    return -1;
	                }
	                temp.erase(pos, 1);
	                S.condition_value.push_back(temp);
	                S.condition_num++;
	                if (sql[0] == ';')
	                {
	                    break;
	                }
	                else
	                {
	                    pos_space = sql.find(' ');
	                    if (pos_space < 0)
	                    {
	                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                        return -1;
	                    }
	                    temp = sql.substr(0, pos_space);
	                    sql.erase(0, pos_space + 1);
	                    removeSpace(sql);
	                    if (temp == lag_and)
	                    {

	                    }
	                    else
	                    {
	                        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	                        return -1;
	                    }
	                }
	            }
	        }
	    }
	    else
	    {
	        cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
	        return -1;
	    }
	}

	return opt;
}

int execfileConvert(string &sql)
{
	int opt = 50;
    string temp;
	string file_pos("./test_file/");

	const auto pos_space = sql.find(' ');
	const auto pos_end = sql.find(';');
	if (pos_space != string::npos)
	{
		temp = sql.substr(0, pos_space);
		sql.erase(0, pos_space + 1);
		removeSpace(sql);
		if (sql[0] == ';')
		{
			file_pos += temp;
			getFileConvert(file_pos);
		}
		else
		{
			cout << "ERROR 0000: You have an error in your SQL syntax." << endl;
			return -1;
		}
	}
	else
	{
		temp = sql.substr(0, pos_end);
		sql.erase(0, pos_end + 1);
		file_pos += temp;
		getFileConvert(file_pos);
	}

	return opt;
}

void getFileConvert(string &file_pos)
{
	ifstream file(file_pos, ios::_Nocreate);
	string temp;
	char c;

	if (!file)
	{
		cout << "ERROR 00002: File open error." << endl;
		return;
	}
	else
	{
		temp.assign("");
		while ((c = file.get()) != EOF)
		{
			if (c == '\n')
			{
				c = ' ';
			}
			temp.append(1, c);
			if (c == ';')
			{
				sqlConvert(temp);
				temp.assign("");
			}
		}
	}

}

//判断执行界面一条sql语句是否输入结束
bool isSqlEnd(string &temp)
{
    // const int pos = temp.find(';', 0);
    return temp.find(';', 0) != string::npos;
    // return 0 <= pos;
}

//删去string头部的所有space
void removeSpace(string &temp)
{
	while (temp[0] == ' ')
	{
		temp.erase(0, 1);
	}
}

//删去string中的所有space
void removeSpaceAll(string &temp)
{
    int pos = temp.find(' ');
	while (pos > 0)
	{
		temp.erase(pos, 1);
		pos = temp.find(' ');
	}
}

void outputError(int opt)
{
	if (opt < 0)
	{
		cout << "Error" << endl;
	}
	else
	{
		switch (opt)
		{
		case 1: cout << "create" << endl; break;
		case 2: cout << "delete" << endl; break;
		case 3: cout << "drop" << endl; break;
		case 4: cout << "insert" << endl; break;
		case 5: cout << "select" << endl; break;
		case 6: cout << "execfile" << endl; break;
		case 7: cout << "quit" << endl; break;
		default: cout << "Error" << endl;
		}
	}
}

