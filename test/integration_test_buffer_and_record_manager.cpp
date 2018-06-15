#include "API.h"
#include "OperationInput.h"
#include "Serialize.h"
#include "buffer.h"

Buffer* database;

int main()
{
    Buffer d;
    database = &d;
    { 
        Insert insert;
        insert.value_num = 4;
        insert.column_num = 4;
        insert.table_name = std::string("Student");
        insert.column_name[0] = std::string("name");
        insert.column_name[1] = std::string("class");
        insert.column_name[2] = std::string("grade");
        insert.column_name[3] = std::string("weight");
        insert.column_type[0] = CHAR;
        insert.column_type[1] = CHAR;
        insert.column_type[2] = INT;
        insert.column_type[3] = FLOAT;
        insert.string_length[0] = 20;
        insert.string_length[1] = 3;
        insert.is_unique[0] = true;
        insert.row_value[0] = "Ray Yan";
        insert.row_value[1] = "s2";
        insert.row_value[2] = "99";
        insert.row_value[3] = "59.52";

        // Table t = insert;  // NOLINT(cppcoreguidelines-slicing)
        std::cout << recordExecute(insert) << endl;
    }

    {
        Insert insert;
        insert.value_num = 4;
        insert.column_num = 4;
        insert.table_name = std::string("Student");
        insert.column_name[0] = std::string("name");
        insert.column_name[1] = std::string("class");
        insert.column_name[2] = std::string("grade");
        insert.column_name[3] = std::string("weight");
        insert.column_type[0] = CHAR;
        insert.column_type[1] = CHAR;
        insert.column_type[2] = INT;
        insert.column_type[3] = FLOAT;
        insert.string_length[0] = 20;
        insert.string_length[1] = 3;
        insert.is_unique[0] = true;
        insert.row_value[0] = "YXTR";
        insert.row_value[1] = "s2";
        insert.row_value[2] = "61";
        insert.row_value[3] = "60";

        // Table t = insert;  // NOLINT(cppcoreguidelines-slicing)
        std::cout << recordExecute(insert) << endl;
    }

    {
        Insert insert;
        insert.value_num = 4;
        insert.column_num = 4;
        insert.table_name = std::string("Student");
        insert.column_name[0] = std::string("name");
        insert.column_name[1] = std::string("class");
        insert.column_name[2] = std::string("grade");
        insert.column_name[3] = std::string("weight");
        insert.column_type[0] = CHAR;
        insert.column_type[1] = CHAR;
        insert.column_type[2] = INT;
        insert.column_type[3] = FLOAT;
        insert.string_length[0] = 20;
        insert.string_length[1] = 3;
        insert.is_unique[0] = true;
        insert.row_value[0] = "Ray Yan";
        insert.row_value[1] = "c1";
        insert.row_value[2] = "75";
        insert.row_value[3] = "69";

        // Table t = insert;  // NOLINT(cppcoreguidelines-slicing)
        std::cout << recordExecute(insert) << endl;
    }
    std::cout << "\n------Testing Select----" << endl;
    {
        Select select;

        select.column_num = 4;
        select.table_name = std::string("Student");
        select.column_name[0] = std::string("name");
        select.column_name[1] = std::string("class");
        select.column_name[2] = std::string("grade");
        select.column_name[3] = std::string("weight");
        select.column_type[0] = CHAR;
        select.column_type[1] = CHAR;
        select.column_type[2] = INT;
        select.column_type[3] = FLOAT;
        select.string_length[0] = 20;
        select.string_length[1] = 3;

        select.condition_num = 1;
        select.condition_name.emplace_back("grade");
        select.condition_op.emplace_back(">");
        select.condition_value.emplace_back("60");

        
        std::cout << recordExecute(select);
    }
    std::cout << "\n------Testing Delete----" << endl;

    {
        Delete delete_;

        delete_.column_num = 4;
        delete_.table_name = std::string("Student");
        delete_.column_name[0] = std::string("name");
        delete_.column_name[1] = std::string("class");
        delete_.column_name[2] = std::string("grade");
        delete_.column_name[3] = std::string("weight");
        delete_.column_type[0] = CHAR;
        delete_.column_type[1] = CHAR;
        delete_.column_type[2] = INT;
        delete_.column_type[3] = FLOAT;
        delete_.string_length[0] = 20;
        delete_.string_length[1] = 3;

        delete_.condition_num = 1;
        delete_.condition_name.emplace_back("grade");
        delete_.condition_op.emplace_back("<");
        delete_.condition_value.emplace_back("70");


        std::cout << recordExecute(delete_) << endl;
    }
    std::cout << "Delete where grade < 70" << endl;
    std::cout << "\n------After Delete----" << endl;
    {
        Select select;

        select.column_num = 4;
        select.table_name = std::string("Student");
        select.column_name[0] = std::string("name");
        select.column_name[1] = std::string("class");
        select.column_name[2] = std::string("grade");
        select.column_name[3] = std::string("weight");
        select.column_type[0] = CHAR;
        select.column_type[1] = CHAR;
        select.column_type[2] = INT;
        select.column_type[3] = FLOAT;
        select.string_length[0] = 20;
        select.string_length[1] = 3;

        select.condition_num = 0;

        std::cout << recordExecute(select) << endl;
    }
}