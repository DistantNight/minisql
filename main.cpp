#include <iostream>
#include "buffer.h"

int main()
{
	Buffer database;
	//���Ի�ÿ�
	blockInfo* block_node = database.get_file_block("test", 0, 0);
	//���Ի������
	char* buffer = database.get_content(block_node);
	for (int i = 0; i < BLOCK_LEN; i++)
		std::cout << buffer[i];
	//���������ݻ�д
	buffer[3] = '4';
	buffer[2] = '3';
	database.set_dirty(block_node, 1);
	//����LRU
	blockInfo* (blocks[MAX_BLOCK + 3]);
	for (int i = 0; i < 2; i++) {
		blocks[i] = database.get_file_block("test", 0, i);
		buffer = database.get_content(blocks[i]);
		database.set_dirty(blocks[i], 1);
		buffer[3] = '4';
		buffer[2] = '3';
	}
	system("pause");
	return 0;
}