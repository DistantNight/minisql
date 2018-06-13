#include <iostream>
#include "buffer.h"

int main()
{
	Buffer database;
	blockInfo* block_node = database.get_file_block("test", 0, 0);
	char* buffer = new char[BLOCK_LEN + 1];
	memcpy(buffer, database.get_content(block_node), BLOCK_LEN);
	std::cout << *buffer << std::endl;
	return 0;
}