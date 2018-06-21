#include "API.h"
#include "index.h"
#include "buffer.h"

extern Buffer* database;

string indexExecute(DropIndex& drop_index)
{
	Buffer& buffer = *database;
	IndexManager index(drop_index);
	string* indexs = drop_index.all_index_name;
    int i = 0;
	for(; i < drop_index.column_num; i++)
	{
		if(indexs[i] == drop_index.index_name) break;			
	}
    if (i >= drop_index.column_num)
    {
        throw std::runtime_error("Index name doesn't exist");
    }
	index.dropIndex(drop_index.index_name, drop_index.column_type[i]);
	buffer.remove_file(drop_index.index_name, 1);
	return "Drop index " + drop_index.index_name + " successfully";
} 

bool indexExecute(DropTable& drop_table)
{
	Buffer& buffer = *database;
	IndexManager index(drop_table);
	string* indexs = drop_table.all_index_name;
	for(int i = 0; i < drop_table.column_num; i++)
	{
		if(!indexs[i].empty())
		{
			index.dropIndex(drop_table.all_index_name[i], drop_table.column_type[i]);
			buffer.remove_file(drop_table.all_index_name[i], 1);
		}			
	}
    return true;
} 


bool indexExecute(CreateTable& table)
{
    IndexManager index_manager(table);
    return true;
}

