#include <iostream>
#include "buffer.h"

int main()
{
	Buffer database;
	//测试获得块
	blockInfo* block_node = database.get_file_block("test", 0, 0);
	//测试获得内容
	char* buffer = database.get_content(block_node);
	for (int i = 0; i < BLOCK_LEN; i++)
		std::cout << buffer[i];
	//测试脏数据回写
	buffer[3] = '4';
	buffer[2] = '3';
	database.set_dirty(block_node, 1);
	//测试LRU
	blockInfo* (blocks[MAX_BLOCK + 3]);
	for (int i = 0; i < 2; i++) {
		blocks[i] = database.get_file_block("test", 0, i);
		buffer = database.get_content(blocks[i]);
		database.set_dirty(blocks[i], 1);
		buffer[3] = '4';
		buffer[2] = '3';
	}
	//测试新开内存块
	blocks[2] = database.get_file_block("test", 0, 2);
	buffer = database.get_content(blocks[2]);
	database.set_dirty(blocks[2], 1);
	buffer[3] = 'A';
	buffer[4] = ' ';
	buffer[5] = 'T';
//	system("pause");
	return 0;
}