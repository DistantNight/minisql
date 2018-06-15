#include <iostream>
#include "buffer.h"

Buffer* database;

int main()
{
	database = new Buffer;
	blockInfo* (blocks[MAX_BLOCK + 3]);
	//���Ի�ÿ�
	blocks[0] = database->get_file_block("test", 0, 0);
	//���Ի������
	char* buffer = database->get_content(blocks[0]);
	for (int i = 0; i < BLOCK_LEN; i++)
		std::cout << buffer[i];
	//���������ݻ�д
	buffer[3] = '4';
	buffer[2] = '3';
	database->set_dirty(blocks[0], 1);
	//����LRU
	for (int i = 0; i < MAX_BLOCK + 2; i++) {
		blocks[i] = database->get_file_block("test", 0, i);
		buffer = database->get_content(blocks[i]);
		database->set_dirty(blocks[i], 1);
		buffer[3] = '4';
		buffer[2] = '3';
	}
	//�����¿��ڴ��
	blocks[2] = database->get_file_block("test", 0, 2);
	buffer = database->get_content(blocks[2]);
	database->set_dirty(blocks[2], 1);
	buffer[3] = 'A';
	buffer[4] = ' ';
	buffer[5] = 'T';
	//���Զ��ļ���
	blocks[2] = database->get_file_block("test_file_1", 0, 0);
	blocks[3] = database->get_file_block("test_file_2", 0, 0);
	blocks[4] = database->get_file_block("test_file_3", 0, 0);
	blocks[5] = database->get_file_block("test_file_4", 0, 0);
	blocks[6] = database->get_file_block("test_file_5", 0, 0);
	return 0;
}