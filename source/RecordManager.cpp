/* 
 * Ray Yan 
 * 2018/6/10
 */

#include "API.h"
#include "OperationInput.h"
#include <cstdint>
#include <exception>
#include <vector>
#include <map>
#include <iomanip>
#include "Serialize.h"


// TODO: delete this array when 
using namespace std;
int8_t page[8192]; // 8k bytes per page
/*
 * Record page: (the number is the offset)
 * 0 - 192 header
 *     0 - 159 table name, type : char[160]
 *     160 - 163 tuple size, type : int
 *      (for example a table with attribute char(5), char(10), int, float should have size of 5 + 10 + 4 + 4 = 23)
 *     164 page indicator, type : int8_t. '0' means an empty memory page, '1' means the first page of the record, 
 *      '2' means the second page of the record and so on.
 *     165 column number, type: int8_t
 *     166 - 169 end of tuple offset
 *     176 - 187  size of each tuple, type: int8_t
 *     
 *     188 - 191 offset to next available space to hold the tuple(free list), type : int
 */

// a function help to determine if the condition is satisfied
template <typename T>
bool checkCondition(const T &v1, const T &v2, string condition_op)
{
    if (condition_op == "=")
    {
        return v1 == v2;
    }
    if (condition_op == "<>")
    {
        return v1 != v2;
    }
    if (condition_op == "<")
    {
        return v1 < v2;
    }
    if (condition_op == ">")
    {
        return v1 > v2;
    }
    if (condition_op == "<=")
    {
        return v1 <= v2;
    }
    if (condition_op == ">=")
    {
        return v1 >= v2;
    }
    return false;
}

string recordExecute(Insert &table)
{
    if (table.value_num != table.column_num)
    {
        throw length_error("value number is not equal to column munber!");
    }
    int8_t *memory_page = page; // TODO: use buffer provided by buffer manager

    if (memory_page == nullptr)
    {
        return string("Unable to read database file");
    }

    // TODO: multipage
    if (memory_page[164] == 0) // a new page, then the function need to add some metadata to the header of the page 
    {
        memory_page[164] = 1; // the first page of this table
        int8_t * offset_current = memory_page; // a pointer point to current location;
        pack(table.table_name, offset_current);
        offset_current += 160;
        

        memory_page[165] = table.column_num;

        offset_current += 16; //[176]
        int tuple_size = 0;
        for (int i = 0; i < table.column_num; ++i)
        {
            switch (table.column_type[i])
            {
            case INT:
                *offset_current = 4;
                tuple_size += 4;
                break;
            case FLOAT:
                *offset_current = 4;
                tuple_size += 4;
                break;
            case CHAR:
                *offset_current = table.string_length[i];
                tuple_size += table.string_length[i];
                break;
            }
            ++offset_current;
        }
        ++tuple_size; // 1 byte for valid bit
        tuple_size += 4; // 4 byte for storing the offset of the next deleted tuple.
        pack(tuple_size, memory_page + 160); //store tuple size at [160 - 163]

        pack(192, memory_page + 166); // "the end of the record the at offset 192." This message is store at memory_page[166-169] as a int
    }

    // TODO: search free list to find place to insert
    // TODO: check unique
    // insert behind
    int8_t *insert_position = memory_page + unpackInt(memory_page + 166); // insert new tuple at the end of previous record
    if (*insert_position == 1) // the position already has a record, should not happen
    {
        throw logic_error("Can not insert record here");
    }
    const int tuple_size = unpackInt(memory_page + 160); // clear tuple_size bytes memory for storing the tuple
    memset(insert_position, 0, tuple_size);
    *insert_position = 1; // set valid bit to 1, means there is a record
    ++insert_position;
    for (int i = 0; i < table.value_num; ++i)
    {
        std::istringstream record(table.row_value[i]);
        switch (table.column_type[i])
        {
        case INT:            
            int int_value;
            record >> int_value;
            pack(int_value, insert_position);
            insert_position += 4;
            break;
        case FLOAT:
            float float_value;
            record >> float_value;
            pack(float_value, insert_position);
            insert_position += 4;
            break;
        case CHAR:
            pack(table.row_value[i], insert_position);
            insert_position += table.string_length[i];
            break;
        }
    }
    const int new_end_of_tuple_offset = unpackInt(memory_page + 166) + tuple_size;
    if (new_end_of_tuple_offset + memory_page != insert_position + 4)
    {
        throw logic_error("size of tuple doesn't match");
    }
    pack(new_end_of_tuple_offset, memory_page + 166);

    return std::string("Insert successfully.");
}

string recordExecute(Select &table)
{
    ostringstream query_result;
    vector<vector<string>> columns(table.column_num); // search result will store here for printing
    vector<unsigned> columns_print_width(table.column_num, 0);
    for (int i = 0; i < table.column_num; ++i)
    {
        columns.at(i).push_back(table.column_name[i]);
        columns_print_width[i] = table.column_name[i].length();
    }

    map<string, int> column_name_to_index;
    for (int i = 0; i < table.column_num; ++i)
    {
        column_name_to_index.insert(make_pair(table.column_name[i], i));
    }

    
    // begin quering
    int result_tuple_num = 1; // 1 for the title

    // TODO: query with index
    // TODO: use buffer provided by buffer manager
    const int8_t *memory_page = page;

    if (memory_page == nullptr)
    {
        return string("Unable to read database file");
    }
    if (memory_page[164] == 0) // new page, empty record
    {
        // drawing header
        ostringstream table_border;
        table_border << '+';
        for (int i = 0; i < table.column_num; ++i)
        {
            const string border(columns_print_width[i] + 2, '-');
            table_border << border;
            table_border << '+';
        }
        query_result << table_border.str() << "\n|"; // e.g. +-----------+------+------------+-----------------+
        for (int i = 0; i < table.column_num; ++i)   // e.g. | City name | Area | Population | Annual Rainfall |
        {
            query_result << ' ';
            query_result << setw(columns_print_width[i]) << columns.at(i)[0]; // header
            query_result << " |";
        }
        query_result << '\n' << table_border.str() << '\n';
        query_result << table_border.str() << "\nTotal tuple(s): " << 0;
        return query_result.str();
    }

    const int tuple_size = unpackInt(memory_page + 160);
    const int8_t *reading_positon = memory_page + 192; // read the memory from here
    
    while (reading_positon < memory_page + unpackInt(memory_page + 166)) // !eof
    {
        bool meet_conditions = true;
        // auto read_here = reading_positon;
        if (reading_positon[0] == 0) // deleted
        {
            continue;
        }
        for (int i = 0; i < table.condition_num; ++i) // examine if this tuple satisfy all the conditions 
        {
            const auto col_name = table.condition_name.at(i);
            auto col_index = column_name_to_index[col_name];            
            int offset = 1; // the first byte is the valid bit; 
            for (int j = 0; j < col_index; ++j) // start reading offset from [176] in header of the memory page
            {
                offset += memory_page[176 + j];
            }

            int value_int, condition_int;
            float value_float, condition_float;
            string str;

            switch (table.column_type[col_index])
            {
            case INT:
                value_int = unpackInt(&reading_positon[offset]);
                condition_int = stoi(table.condition_value.at(i));
                meet_conditions = checkCondition(value_int, condition_int, table.condition_op.at(i));
                break;
            case FLOAT:
                value_float = unpackFloat(&reading_positon[offset]);
                condition_float = stof(table.condition_value.at(i));
                meet_conditions = checkCondition(value_float, condition_float, table.condition_op.at(i));
                break;
            case CHAR:
                str = unpackString(&reading_positon[offset], table.string_length[col_index]);
                meet_conditions = checkCondition(str, table.condition_value.at(i), table.condition_op.at(i));
                break;
            }
            if (!meet_conditions)
            {
                break;
            }
        }

        if (meet_conditions) // this tuple will be showed in the query result
        {
            for (int i = 0; i < table.column_num; ++i) // put values in to the table
            {
                string value_on_screen;

                // same routine to get value
                int offset = 1; // the first byte is the valid bit; 
                for (int j = 0; j < i; ++j) // start reading offset from [176] in header of the memory page
                {
                    offset += memory_page[176 + j];
                }

                int value_int;
                float value_float;
                string str;
                switch (table.column_type[i])
                {
                case INT:
                    value_int = unpackInt(&reading_positon[offset]);
                    value_on_screen = std::to_string(value_int);
                    break;
                case FLOAT:
                    value_float = unpackFloat(&reading_positon[offset]);
                    value_on_screen = std::to_string(value_float);
                    break;
                case CHAR:
                    str = unpackString(&reading_positon[offset], table.string_length[i]);
                    value_on_screen = str;
                    break;
                }
                columns.at(i).push_back(value_on_screen);
                if (columns_print_width[i] < value_on_screen.length())
                {
                    columns_print_width[i] = value_on_screen.length();
                }                 
            }
            ++result_tuple_num;
        }
        reading_positon += tuple_size;
    }

    // drawing header
    ostringstream table_border;
    table_border << '+';
    for (int i = 0; i < table.column_num; ++i)
    {
        const string border(columns_print_width[i] + 2, '-');
        table_border << border;
        table_border << '+';
    }
    query_result << table_border.str() << "\n|"; // e.g. +-----------+------+------------+-----------------+
    for (int i = 0; i < table.column_num; ++i)   // e.g. | City name | Area | Population | Annual Rainfall |
    {
        query_result << ' ';
        query_result << setw(columns_print_width[i]) << columns.at(i)[0]; // header
        query_result << " |";
    }
    query_result << '\n' << table_border.str() << '\n';

    // drawing table body
    for (int j = 1; j < result_tuple_num; ++j)
    {
        query_result << '|';
        for (int i = 0; i < table.column_num; ++i)
        {
            query_result << ' ';
            query_result << setw(columns_print_width[i]) << columns.at(i).at(j); // body
            query_result << " |";
        }
        query_result << '\n';
    }
    query_result << table_border.str() << "\nTotal tuple(s): " << result_tuple_num - 1;
    return query_result.str();
}