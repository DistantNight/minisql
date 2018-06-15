/* 
 * Ray Yan 
 * 2018/6/15
 */

#include "RecordManager.h"
#include "API.h"
#include "OperationInput.h"
#include <cstdint>
#include <vector>
#include <map>
#include <iomanip>
#include "Serialize.h"
#include "buffer.h"

extern Buffer* database;
using namespace std;
const int page_size = BLOCK_LEN;

/*
 * Record page: (the number is the offset)
 * 0 - 191 header
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
bool checkCondition(const T &v1, const T &v2, const string& condition_op)
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

RecordManager::RecordManager(const Table & table): table_(table)
{
    int size = 0;
    for (int i = 0; i < table.column_num; ++i)
    {
        switch (table.column_type[i])
        {
        case INT:
            size += 4;
            break;
        case FLOAT:

            size += 4;
            break;
        case CHAR:
            
            size += table.string_length[i];
            break;
        }

        column_name_to_index.insert(make_pair(table_.column_name[i], i));
    }
    ++size; // 1 byte for valid bit
    size += 4; // 4 byte for storing the offset of the next deleted tuple.
    tuple_size = size;
}

bool RecordManager::isEmptyPage() const
{    
    return memory_page_[164] == 0;
}

void RecordManager::addMetadataToPage() const
{
    memory_page_[164] = 1; // the first page of this table
    int8_t * offset_current = memory_page_; // a pointer point to current location;
    pack(table_.table_name, offset_current);
    offset_current += 160;


    memory_page_[165] = table_.column_num;

    offset_current += 16; //[176]
    int tuple_size = 0;
    for (int i = 0; i < table_.column_num; ++i)
    {
        switch (table_.column_type[i])
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
            *offset_current = table_.string_length[i];
            tuple_size += table_.string_length[i];
            break;
        }
        ++offset_current;
    }
    ++tuple_size; // 1 byte for valid bit
    tuple_size += 4; // 4 byte for storing the offset of the next deleted tuple.
    pack(tuple_size, memory_page_ + 160); //store tuple size at [160 - 163]

    pack(192, memory_page_ + 166); // "the end of the record the at offset 192." This message is store at memory_page[166-169] as a int
}

int RecordManager::getEndofTupleOffset() const
{
    return unpackInt(memory_page_ + 166);
}

void RecordManager::setEndofTupleOffset(int offset) const
{
    pack(offset, memory_page_ + 166); // update end()

}

int RecordManager::getTuplesPerPage() const
{
    return (page_size - 192) / tuple_size;
}

int8_t * RecordManager::begin() const
{
    return memory_page_ + 192;
}

int8_t * RecordManager::end() const
{
    return memory_page_ + getEndofTupleOffset();
}

void RecordManager::toNextPage()
{
    ++current_page_index_;
    const auto block = database->get_file_block(table_.table_name, false, current_page_index_);
    const auto page = database->get_content(block);
    memory_page_ = reinterpret_cast<int8_t*> (page);

    if (memory_page_ == nullptr)
    {
        throw runtime_error("Unable to read database file");
    }
}

void RecordManager::toPreviousPage()
{
    if (current_page_index_ == 0)
    {
        throw logic_error("Can not go to previous page because current page is the first page");
    }
    --current_page_index_;
    const auto page = database->get_file_block(table_.table_name, false, current_page_index_);
    memory_page_ = reinterpret_cast<int8_t*> (database->get_content(page));
    if (memory_page_ == nullptr)
    {
        throw runtime_error("Unable to read database file");
    }
}

bool RecordManager::haveSpaceToInsert() const
{
    return end() + tuple_size < memory_page_ + page_size;
}

std::vector<std::string> RecordManager::getTuple(const int tuple_i) const
{
    if (tuple_i < 0)
    {
        return std::vector<std::string>();
    }
    if (end() < begin() + tuple_i * tuple_size)
    {
        return std::vector<std::string>();
    }
    std::vector<std::string> result(table_.column_num);
    auto reading_position = begin() + tuple_i * tuple_size;

    for (int i = 0; i < table_.column_num; ++i) // put values in to the table
    {
        // string value_on_screen;

        // same routine to get value
        int offset = 1; // the first byte is the valid bit; 
        for (int j = 0; j < i; ++j) // start reading offset from [176] in header of the memory page
        {
            offset += memory_page_[176 + j];
        }

        int value_int;
        float value_float;
        string str;
        switch (table_.column_type[i])
        {
        case INT:
            value_int = unpackInt(&reading_position[offset]);
            result.at(i) = std::to_string(value_int);
            break;
        case FLOAT:
            value_float = unpackFloat(&reading_position[offset]);
            result.at(i) = std::to_string(value_float);
            break;
        case CHAR:
            str = unpackString(&reading_position[offset], table_.string_length[i]);
            result.at(i) = str;
            break;
        }
    }
    return result;
}

bool RecordManager::tupleDeleted(int tuple_i) const
{
    const auto reading_position = begin() + tuple_i * tuple_size;
    return reading_position[0] == 0;
}

void RecordManager::lazyDeleteTuple(int tuple_i) const
{
    const auto reading_position = begin() + tuple_i * tuple_size;
    reading_position[0] = 0;
}

void RecordManager::tellBufferPageHasChanged() const
{
    database->set_dirty(database->get_file_block(table_.table_name, false, current_page_index_), true);
}


bool RecordManager::passUniqueConstraintTest(const Insert& table, const RecordManager& r)
{
    for (int i = 0; i < (r.getEndofTupleOffset() - 192) / r.tuple_size; ++i)
    {
        if (r.tupleDeleted(i))
        {
            continue;
        }
        auto result = r.getTuple(i);
        for (int j = 0 ; j < table.column_num; ++j)
        {
            if (table.is_unique[j] && result.at(j) == table.row_value[j]) // should be unique, but insert same string
            {
                return false;
            }
        }
    }
    return true;
}

string recordExecute(Insert &table)
{
    if (table.value_num != table.column_num)
    {
        throw length_error("value number is not equal to column munber!");
    }
    RecordManager r(table);
    //int8_t *memory_page = page;

/*    if (memory_page == nullptr)
    {
       return string("Unable to read database file");
    }*/ 

    while (true)
    {
        r.toNextPage();
        if (r.isEmptyPage()) // a new page, then the function need to add some metadata to the header of the page 
        {
            r.addMetadataToPage();
        }
        if (RecordManager::passUniqueConstraintTest(table, r))
        {
            if (r.haveSpaceToInsert())
            {
                // insert behind
                auto *insert_position = r.end();
                if (*insert_position == 1) // the position already has a record, should not happen
                {
                    throw logic_error("Can not insert record here");
                }

                std::memset(insert_position, 0, r.tuple_size);
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
                const int new_end_of_tuple_offset = r.getEndofTupleOffset() + r.tuple_size;
                //if (new_end_of_tuple_offset + memory_page_ != insert_position + 4) // 4 bytes are the reserve space
                //{
                //    throw logic_error("size of tuple doesn't match");
                //}
                /*pack(new_end_of_tuple_offset, memory_page_ + 166);*/ // update end()
                r.setEndofTupleOffset(new_end_of_tuple_offset);
                r.tellBufferPageHasChanged();
                return std::string("Query OK, 1 row affected");
            }
            else
            {
                continue;
            }
        }
        else
        {
            return std::string("ERROR 1551: Cannot insert a row: a unique key constraint fails");
        }
    } 
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

    RecordManager r(table);

    /*map<string, int> column_name_to_index;
    for (int i = 0; i < table.column_num; ++i)
    {
        column_name_to_index.insert(make_pair(table.column_name[i], i));
    }*/
    
    r.toNextPage();
    if (r.isEmptyPage()) // new page, empty record
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
        query_result << table_border.str() << "\n0 row in set";
        return query_result.str();
    }

    // TODO: other quering


    // begin sequential quering
    int result_tuple_num = 1; // 1 for the title

    do
    {
        const bool last_page = r.haveSpaceToInsert(); // if it has space to insert, then it must be the last page
        if (r.isEmptyPage())
        {
            break;
        }
        const int8_t *reading_positon = r.begin(); // read the memory from here
        int tuple_i = 0;
        while (reading_positon < r.end()) // !eof
        {
            bool meet_conditions = true;
            // auto read_here = reading_positon;
            if (r.tupleDeleted(tuple_i)) // deleted
            {
                reading_positon += r.tuple_size;
                ++tuple_i;
                continue;
            }
            auto tuple = r.getTuple(tuple_i);
            for (int i = 0; i < table.condition_num; ++i) // examine if this tuple satisfy all the conditions 
            {
                const auto col_name = table.condition_name.at(i);
                const auto col_index = r.column_name_to_index[col_name];
                //int offset = 1; // the first byte is the valid bit; 
                

                int value_int, condition_int;
                float value_float, condition_float;
                string str;

                switch (table.column_type[col_index])
                {
                case INT:
                    value_int = stoi(tuple.at(col_index));
                    condition_int = stoi(table.condition_value.at(i));
                    meet_conditions = checkCondition(value_int, condition_int, table.condition_op.at(i));
                    break;
                case FLOAT:
                    value_float = stof(tuple.at(col_index));
                    condition_float = stof(table.condition_value.at(i));
                    meet_conditions = checkCondition(value_float, condition_float, table.condition_op.at(i));
                    break;
                case CHAR:
                    str = tuple.at(col_index);
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
                    const string value_on_screen = tuple[i];
                    columns.at(i).push_back(value_on_screen);
                    if (columns_print_width[i] < value_on_screen.length())
                    {
                        columns_print_width[i] = value_on_screen.length();
                    }
                }
                ++result_tuple_num;
            }
            reading_positon += r.tuple_size;
            ++tuple_i;
        }
        if (last_page)
        {
            break;
        }
        r.toNextPage();
    } while (true);
    // const int tuple_size = unpackInt(memory_page + 160);
    
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
        query_result << setw(columns_print_width.at(i)) << columns.at(i).at(0); // header
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
    query_result << table_border.str() << "\n" << result_tuple_num - 1 << "row(s) in set";
    return query_result.str();
}

string recordExecute(Delete &table) // delete
{
    RecordManager r(table);

    int result_tuple_num = 0; 

    r.toNextPage();
    if (r.isEmptyPage())
    {
        return string("Query OK, 0 row affected.");
    }

    // begin sequential quering
    do
    {
        const bool last_page = r.haveSpaceToInsert(); // if it has space to insert, then it must be the last page
        if (r.isEmptyPage())
        {
            break;
        }
        const int8_t *reading_positon = r.begin(); // read the memory from here
        int tuple_i = 0;
        while (reading_positon < r.end()) // !eof
        {
            bool meet_conditions = true;
            // auto read_here = reading_positon;
            if (r.tupleDeleted(tuple_i)) // deleted
            {
                reading_positon += r.tuple_size;
                ++tuple_i;
                continue;
            }
            auto tuple = r.getTuple(tuple_i);
            for (int i = 0; i < table.condition_num; ++i) // examine if this tuple satisfy all the conditions 
            {
                const auto col_name = table.condition_name.at(i);
                const auto col_index = r.column_name_to_index[col_name];

                int value_int, condition_int;
                float value_float, condition_float;
                string str;

                switch (table.column_type[col_index])
                {
                case INT:
                    value_int = stoi(tuple.at(col_index));
                    condition_int = stoi(table.condition_value.at(i));
                    meet_conditions = checkCondition(value_int, condition_int, table.condition_op.at(i));
                    break;
                case FLOAT:
                    value_float = stof(tuple.at(col_index));
                    condition_float = stof(table.condition_value.at(i));
                    meet_conditions = checkCondition(value_float, condition_float, table.condition_op.at(i));
                    break;
                case CHAR:
                    str = tuple.at(col_index);
                    meet_conditions = checkCondition(str, table.condition_value.at(i), table.condition_op.at(i));
                    break;
                }
                if (!meet_conditions)
                {
                    break;
                }
            }

            if (meet_conditions) // this tuple will deleted
            {
               // delete the tuple
                r.lazyDeleteTuple(tuple_i);
                ++result_tuple_num;
            }
            reading_positon += r.tuple_size;
            ++tuple_i;
        }
        if (last_page)
        {
            break;
        }
        r.tellBufferPageHasChanged();
        r.toNextPage();
    } while (true);

    ostringstream deleting_result;
    deleting_result << "Query OK, " << result_tuple_num << "row(s) affected";

    return deleting_result.str();
}

