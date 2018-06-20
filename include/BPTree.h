#ifndef _BPTREE_
#define _BPTREE_

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "buffer.h"


#define  SMALL_AMOUNT  			30                                  //少于这个数量，直接遍历 

const string ROOT = "./data/";										//这个目录之后统一一下,放索引文件的 
extern Buffer* database;

template <typename T>
void copyStr(char* p , int& offset , T data)
{
    std::stringstream stream;
    stream << data;
    std::string s = stream.str();
    for(int i = 0; i < s.length(); i++, offset++) p[offset] = s[i];
}

//B+树节点类定义 

template <typename T>
class BPT_Node{
	public:
		int 					degree;			 
		bool 					is_leaf;		 
		unsigned int 			num_of_key;		 

		BPT_Node*				parent;			 
		BPT_Node*				next;			 

		std::vector<T> 			keys;			 
		std::vector<int>		values;    		//其中value是指该key的数据所存放在的Block_num 
		std::vector<BPT_Node*> 	childs;			 

	public:
		BPT_Node(int degree, bool is_leaf = false);
		~BPT_Node();
		bool 					isRoot(); 
		bool 					isKeyExist(T key, unsigned int &index);
		BPT_Node* 				cutNode(T &key);
		unsigned int 			addKey(T &key);
		unsigned int 			addKey(T &key, int value);
		bool 					deleteKeyByIndex(unsigned int index);
		BPT_Node* 				nextLeafNode();
		bool 					findBelowKey(unsigned int index, T& key, std::vector<int>& vals);
		bool 					findInRange(unsigned int index, std:: vector<int>& vals);
		void 					printNode();
};

template <typename T>
class BPTree{
	private:
		typedef BPT_Node<T>* BPT;
		struct tempSearch{
			BPT 				temp_node;
			unsigned int 		temp_index;
			bool				is_exist;
		};

	private:
		std::string				file_name;
		BPT 					root;
		BPT 					leaf_head;
		unsigned int 			key_amount;
		unsigned int 			level;
		unsigned int    		node_amount;
		int 					degree;

	public:
		BPTree(std::string name, int degree);
		~BPTree();

		int 					searchValueWithKey(T &key);
		bool 					insertKey(T &key, int value);
		bool 					deleteKey(T &key);
		void 					dropBPT(BPT node);
		void 					searchInRange(T &keyA, T &keyB, std::vector<int>& values, int flag);
		void 					readAllData();
		void 					writeBack();
		void 					readFromDisk(char *ph, char* end);
		void 					printTree();

	private:
		void 					initBPT();
		bool 					adjustAInsert(BPT node);
		bool 					adjustADelete(BPT node);
		void 					findLeafWithKey(BPT node, T key, tempSearch &tS);
		void 					getFile(std::string file_path);
		int 					getBlockNum(std::string file_name);	
};		



template <class T>
BPT_Node<T>::BPT_Node(int degree, bool is_leaf):
		num_of_key(0),
		parent(NULL),
		next(NULL),
		is_leaf(is_leaf),
		degree(degree)
{
	for(unsigned int i = 0; i < degree + 1; i++)
	{
		childs.push_back(NULL);
		keys.push_back(T());
		values.push_back(int());
	}
	childs.push_back(NULL);
}

template <class T>
BPT_Node<T>::~BPT_Node(){}

template <class T>
bool BPT_Node<T>::isRoot()
{
	return (parent == NULL);
}

template <class T>
bool BPT_Node<T>::isKeyExist(T key, unsigned int &index)
{
	if(num_of_key == 0)
	{
		index = 0;
		return false;
	}
	else
	{
		if(keys[num_of_key - 1] < key)
		{
			index = num_of_key;
			return false;
		}
		else if(keys[0] > key)
		{
			index = 0;
			return false;
		}
		else if(num_of_key <= SMALL_AMOUNT)
		{
			for(unsigned int i = 0; i < num_of_key; i++)
			{
				if(keys[i] == key)
				{
					index = i;
					return true;
				}
				else if(keys[i] < key) continue;
				else
				{
					index = i;
					return false;
				}
			}
		}
		else if(num_of_key > SMALL_AMOUNT)
		{
			unsigned int left = 0;
			unsigned int right = num_of_key - 1;
			unsigned int position = 0;

			while(right > left + 1)
			{
				position = (right + left) / 2;
				if(keys[position] == key)
				{
					index = position;
					return true;
				}
				else if(keys[position] < key) left = position;
				else right = position;
			}

			if(keys[left] >= key)
			{
				index = left;
				return (keys[left] == key);
			}
			else if(keys[right] >= key)
			{
				index = right;
				return (keys[right] == key);
			}
			else if(keys[right] < key)
			{
				index = right + 1;
				return false;
			}
		}
	}
	return false;
}

template <class T>
BPT_Node<T>* BPT_Node<T>::cutNode(T &key)
{
	unsigned int minNodeAmount = (degree - 1) / 2;
	BPT_Node* new_node = new BPT_Node(degree, this -> is_leaf);
	
	if(is_leaf)
	{
		key = keys[minNodeAmount + 1];
		for(unsigned int i = minNodeAmount + 1; i < degree; i++)
		{
			new_node -> keys[i - minNodeAmount - 1] = keys[i];
			keys[i] = T();
			new_node -> values[i - minNodeAmount - 1] = values[i];
			values[i] = int();
		}
		new_node -> next = this -> next;
		this -> next = new_node;
		new_node -> parent = this -> parent;
		
		new_node -> num_of_key = degree / 2;
		this -> num_of_key = minNodeAmount + 1;
	}
	else if(!is_leaf)
	{
		key = keys[minNodeAmount];
		for(unsigned int i = minNodeAmount + 1; i < degree + 1; i++)
		{
			new_node -> childs[i - minNodeAmount - 1] = this -> childs[i];
			new_node -> childs[i - minNodeAmount - 1] -> parent = new_node;
			this -> childs[i] = NULL;
		}
		for(unsigned int i = minNodeAmount + 1; i < degree; i++)
		{
			new_node -> keys[i - minNodeAmount - 1] = this -> keys[i];
			this -> keys[i] = T();
		}
		this -> keys[minNodeAmount] = T();
		new_node -> parent = this -> parent;
		
		new_node -> num_of_key = degree / 2;
		this -> num_of_key = minNodeAmount;
	}
	return new_node;
}

template <class T>
unsigned int BPT_Node<T>::addKey(T &key)
{
	if(num_of_key == 0)
	{
		keys[0] = key;
		num_of_key++;
		return 0;
	}
	else
	{
		unsigned int index = 0;
		bool there_is = isKeyExist(key, index);
		if(there_is) return -1;
		else
		{
			for(unsigned int i = num_of_key; i > index; i--) keys[i] = keys[i - 1];
			keys[index] = key;
			for(unsigned int i = num_of_key + 1; i > index + 1; i--) childs[i] = childs[i - 1];
			childs[index + 1] = NULL;
			num_of_key++;
			return index;
		}
	}
	return 0;
}

template <class T>
unsigned int BPT_Node<T>::addKey(T &key, int value)
{
	if(!is_leaf) return -1;
	if(num_of_key == 0)
	{
		keys[0] = key;
		values[0] = value;
		num_of_key++;
		return 0;
	}
	else
	{
		unsigned int index = 0;
		bool there_is = isKeyExist(key, index);
		if(there_is) return -1;
		else
		{
			for(unsigned int i = num_of_key; i > index; i--)
			{
				keys[i] = keys[i - 1];
				values[i] = values[i - 1];
			}
			keys[index] = key;
			values[index] = value;
			num_of_key++;
			return index;
		}
	}
	return 0;
}

template <class T>
bool BPT_Node<T>::deleteKeyByIndex(unsigned int index)
{
	if(index > num_of_key) return false;
	else
	{
		if(is_leaf)
		{
			for(unsigned int i = index; i < num_of_key - 1; i++)
			{
				keys[i] = keys[i + 1];
				values[i] = values[i + 1];
			}
			keys[num_of_key - 1] = T();
			values[num_of_key - 1] = int();
		}
		else
		{
			for(unsigned int i = index; i < num_of_key - 1; i++) keys[i] = keys[i + 1];
			for(unsigned int i = index + 1; i < num_of_key; i++) childs[i] = childs[i + 1];
			keys[num_of_key - 1] = T();
			childs[num_of_key] = NULL;
		}
		num_of_key--;
		return true;
	}
	return false;
}

template <class T>
BPT_Node<T>* BPT_Node<T>::nextLeafNode()
{
	return next;
}

template <class T>
bool BPT_Node<T>::findBelowKey(unsigned int index, T& key, std::vector<int>& vals)
{
	unsigned int i;
	for(i = index; i < num_of_key && keys[i] <= key; i++)
	{
		vals.push_back(values[i]);
	}
	if(keys[i] >= key) return true;
	else return false;
}

template <class T>
bool BPT_Node<T>::findInRange(unsigned int index, std:: vector<int>& vals)
{
	for(unsigned int i = index; i < num_of_key; i++)
	{
		vals.push_back(values[i]);
	}
	return false;
}

template <class T>
BPTree<T>::BPTree(std::string name, int degree):
	file_name(name),
	key_amount(0),
	level(0),
	node_amount(0),
	root(NULL),
	leaf_head(NULL),
	degree(degree)
{
	initBPT();
	readAllData();
}

template <class T>
BPTree<T>::~BPTree()
{
	dropBPT(root);
	key_amount = 0;
	root = NULL;
	level = 0;
}

template <class T>
void BPTree<T>::initBPT()
{
	root = new BPT_Node<T>(degree, true);
	key_amount = 0;
	level = 1;
	node_amount = 1;
	leaf_head = root;
}

template <class T>
void BPTree<T>::findLeafWithKey(BPT node, T key, tempSearch &tS)
{
	unsigned int index = 0;
	if(node -> isKeyExist(key, index))
	{
		if(node -> is_leaf)
		{
			tS.temp_node = node;
			tS.temp_index = index;
			tS.is_exist = true;
		}
		else
		{
			node = node -> childs[index + 1];
			while(!node -> is_leaf)
			{
				node = node -> childs[0];
			}
			tS.temp_node = node;
			tS.temp_index = 0;
			tS.is_exist = true;
		}
	}
	else
	{
		if(node -> is_leaf)
		{
			tS.temp_node = node;
			tS.temp_index = index;
			tS.is_exist = false;
		}
		else
		{
			findLeafWithKey(node -> childs[index], key, tS);
		}
	}
	return;
}

template <class T>
bool BPTree<T>::insertKey(T &key, int value)
{
	tempSearch tS;
	if(!root) initBPT();
	findLeafWithKey(root, key, tS);
	if(tS.is_exist)
	{
		return false;
	}
	else
	{
		tS.temp_node -> addKey(key, value);
		if(tS.temp_node -> num_of_key == degree)
		{
			adjustAInsert(tS.temp_node);
		}
		key_amount++;
		return true;
	}
	return false;
}

template <class T>
bool BPTree<T>::adjustAInsert(BPT node)
{
	T key;
	BPT new_node = node -> cutNode(key);
	node_amount++;
	
	if(node -> isRoot())
	{
		BPT root = new BPT_Node<T>(degree, false);
		if(root == NULL) return false;
		else
		{
			level++;
			node_amount++;
			this -> root = root;
			node -> parent = root;
			new_node -> parent = root;
			root -> addKey(key);
			root -> childs[0] = node;
			root -> childs[1] = new_node;
			return true;
		}
	}
	else
	{
		BPT parent = node -> parent;
		unsigned int index = parent -> addKey(key);
		
		parent -> childs[index + 1] = new_node;
		new_node -> parent = parent;
		
		if(parent -> num_of_key == degree)
		{
			return adjustAInsert(parent);
		}
		return true;
	}
	return false;
}

template <class T>
int BPTree<T>::searchValueWithKey(T &key)
{
	if(!root) return -1;
	tempSearch tS;
	findLeafWithKey(root, key, tS);
	
	if(!tS.is_exist) return -1;
	else return tS.temp_node -> values[tS.temp_index];
	
}

template <class T>
bool BPTree<T>::deleteKey(T &key)
{
	tempSearch tS;
	if(!root) return false;
	else
	{
		findLeafWithKey(root, key, tS);
		if(!tS.is_exist)
		{
			return false;
		}
		else
		{
			if(tS.temp_node -> isRoot())
			{
				tS.temp_node -> deleteKeyByIndex(tS.temp_index);
				key_amount--;
				return adjustADelete(tS.temp_node);
			}
			else
			{
				if(tS.temp_index == 0 && leaf_head != tS.temp_node)
				{
					unsigned int index = 0;
					
					BPT cur_parent = tS.temp_node -> parent;
					bool is_found = cur_parent -> isKeyExist(key, index);
					while(!is_found)
					{
						if(cur_parent -> parent)
						{
							cur_parent = cur_parent -> parent;
						}
						else
						{
							break;
						}
						is_found = cur_parent -> isKeyExist(key, index);
					}
					cur_parent -> keys[index] = tS.temp_node -> keys[1];
					
					tS.temp_node -> deleteKeyByIndex(tS.temp_index);
					key_amount--;
					return adjustADelete(tS.temp_node);
				}
				else
				{
					tS.temp_node -> deleteKeyByIndex(tS.temp_index);
					key_amount--;
					return adjustADelete(tS.temp_node);
				}
			}
		}
	}
	return false;
}

template <class T>
bool BPTree<T>::adjustADelete(BPT node)
{
	unsigned int minNodeAmount = (degree - 1) / 2;
	if(((node -> is_leaf) && (node -> num_of_key >= minNodeAmount)) || 
		 ((degree != 3) && (!node -> is_leaf) && (node -> num_of_key >= minNodeAmount - 1)) ||
			 ((degree == 3) && (!node -> is_leaf) && (node -> num_of_key < 0)))
	{
		return true;
	}
	if(node -> isRoot())
	{
		if(node -> num_of_key > 0) return true;
		else
		{
			if(root -> is_leaf)
			{
				delete node;
				root = NULL;
				leaf_head = NULL;
				level--;
				node_amount--;
			}
			else
			{
				root = node -> childs[0];
				root -> parent = NULL;
				delete node;
				level--;
				node_amount--;
			}
		}
	}
	else
	{
		BPT parent = node -> parent;
		BPT brother = NULL;
		
		if(node -> is_leaf)
		{
			unsigned int index = 0;
			parent -> isKeyExist(node -> keys[0], index);
			
			if((parent -> childs[0] != node) && (index + 1 == parent -> num_of_key))
			{
				brother = parent -> childs[index];
				if(brother -> num_of_key > minNodeAmount)
				{
					for(unsigned int i = node -> num_of_key; i > 0; i--)
					{
						node -> keys[i] = node -> keys[i - 1];
						node -> values[i] = node -> values[i - 1];
					}
					node -> keys[0] = brother -> keys[brother -> num_of_key - 1];
					node -> values[0] = brother -> values[brother -> num_of_key - 1];
					brother -> deleteKeyByIndex(brother -> num_of_key - 1);
					
					node -> num_of_key++;
					parent -> keys[index] = node -> keys[0];
					return true;
				}
				else
				{
					parent -> deleteKeyByIndex(index);
					
					for(unsigned int i = 0; i < node -> num_of_key; i++)
					{
						brother -> keys[i + brother -> num_of_key] = node -> keys[i];
						brother -> values[i + brother -> num_of_key] = node -> values[i];
					}
					brother -> num_of_key += node -> num_of_key;
					brother -> next = node -> next;
					
					delete node;
					node_amount--;
					
					return adjustADelete(parent);
				}
			}
			else
			{
				if(parent -> childs[0] == node) brother = parent -> childs[1];
				else brother = parent -> childs[index + 2];
				
				if(brother -> num_of_key > minNodeAmount)
				{
					node -> keys[node -> num_of_key] = brother -> keys[0];
					node -> values[node -> num_of_key] = brother -> values[0];
					node -> num_of_key++;
					brother -> deleteKeyByIndex(0);
					
					if(parent -> childs[0] == node) parent -> keys[0] = brother -> keys[0];
					else parent -> keys[index + 1] = brother -> keys[0];
					return true;
				}
				else
				{
					for(unsigned int i = 0; i < brother -> num_of_key; i++)
					{
						node -> keys[node -> num_of_key + i] = brother -> keys[i];
						node -> values[node -> num_of_key + i] = brother -> values[i];
					}
					if(node == parent -> childs[0]) parent -> deleteKeyByIndex(0);
					else parent -> deleteKeyByIndex(index + 1);
					node -> num_of_key += brother -> num_of_key;
					node -> next = brother -> next;
					delete brother;
					node_amount--;
					
					return adjustADelete(parent);
				}
			}
		}
		else
		{
			unsigned int index = 0;
			parent -> isKeyExist(node -> childs[0] -> keys[0], index);
			if((parent -> childs[0] != node) && (index + 1 == parent -> num_of_key))
			{
				brother = parent -> childs[index];
				if(brother -> num_of_key > minNodeAmount - 1)
				{
					node -> childs[node -> num_of_key + 1] = node -> childs[node -> num_of_key];
					for(unsigned int i = node -> num_of_key; i > 0; i--)
					{
						node -> childs[i] = node -> childs[i - 1];
						node -> keys[i] = node -> keys[i - 1];
					}
					node -> childs[0] = brother -> childs[brother -> num_of_key];
					node -> keys[0] = parent -> keys[index];
					node -> num_of_key++;
					
					parent -> keys[index] = brother -> keys[brother -> num_of_key - 1];
					
					if(brother -> childs[brother -> num_of_key])
					{
						brother -> childs[brother -> num_of_key] -> parent = node;
					}
					brother -> deleteKeyByIndex(brother -> num_of_key - 1);
					
					return true;
				}
				else
				{
					brother -> keys[brother -> num_of_key] = parent -> keys[index];
					parent -> deleteKeyByIndex(index);
					brother -> num_of_key++;
					
					for(unsigned int i = 0; i < node -> num_of_key; i++)
					{
						brother -> childs[brother -> num_of_key + i] = node -> childs[i];
                        brother -> keys[brother -> num_of_key + i] = node-> keys[i];
                        brother -> childs[brother -> num_of_key + i] -> parent = brother;
					}
					brother -> childs[brother -> num_of_key + node -> num_of_key] = node -> childs[node -> num_of_key];
					brother -> childs[brother -> num_of_key + node -> num_of_key] -> parent = brother;
					
					brother -> num_of_key += node -> num_of_key;
					
					delete node;
					node_amount--;
					
					return adjustADelete(parent);
				}
			}
			else
			{
				if(parent -> childs[0] == node) brother = parent -> childs[1];
				else brother = parent -> childs[index + 2];
				if(brother -> num_of_key > minNodeAmount - 1)
				{
					node -> childs[node -> num_of_key + 1] = brother -> childs[0];
					node -> keys[node -> num_of_key] = brother -> keys[0];
					node -> childs[node -> num_of_key + 1] -> parent = node;
					node -> num_of_key++;
					
					if(node == parent -> childs[0]) parent -> keys[0] = brother -> keys[0];
					else parent -> keys[index + 1] = brother -> keys[0];
					
					brother -> childs[0] = brother -> childs[1];
					brother -> deleteKeyByIndex(0);
					
					return true;
				}
				else
				{
					node -> keys[node -> num_of_key] = parent -> keys[index];
					if(node == parent -> childs[0]) parent -> deleteKeyByIndex(0);
					else parent -> deleteKeyByIndex(index + 1);
					
					node -> num_of_key++;
					
					for(unsigned int i = 0; i < brother -> num_of_key; i++)
					{
						node -> childs[node -> num_of_key + i] = brother -> childs[i];
						node -> keys[node -> num_of_key + i] = brother -> keys[i];
						node -> childs[node -> num_of_key + i] -> parent = node;
					}
					node -> childs[node -> num_of_key + brother -> num_of_key] = brother -> childs[brother -> num_of_key];
					node -> childs[node -> num_of_key + brother -> num_of_key] -> parent = node;
					
					node -> num_of_key += brother -> num_of_key;
					delete brother;
					node_amount--;
					return adjustADelete(parent);
				}
			}
		}
	}
	return false;
}

template <class T>
void BPTree<T>::dropBPT(BPT node)
{
	if(!node) return;
	if(!node -> is_leaf)
	{
		for(unsigned int i = 0; i <= node -> num_of_key; i++)
		{
			dropBPT(node -> childs[i]);
			node -> childs[i] = NULL;
		}
	}
	delete node;
	node_amount--;
	return;
}

template <class T>
void BPTree<T>::printTree()
{
	int flag = 0;
	BPT p = leaf_head;
	while(p != NULL)
	{
		if(flag) std::cout << "↓" << std::endl;
		flag = 1;
		p -> printNode();
		p = p -> nextLeafNode();
	}
	std::cout << std::endl;
	return;
}

template <class T>
void BPT_Node<T>::printNode()
{
	for(unsigned int i = 0; i < num_of_key; i++)
	{
		if(i == 0) std::cout << keys[i];
		else std::cout << " -> " << keys[i];
	}
	std::cout << std::endl;
}

template <class T>
void BPTree<T>::getFile(std::string file_path) 
{
    FILE* f = fopen(file_path.c_str() , "r");
    if (f == NULL) 
	{
        f = fopen(file_path.c_str(), "w+");
        fclose(f);
        f = fopen(file_path.c_str() , "r");
    }
    fclose(f);
    return;
}

template <class T>
int BPTree<T>::getBlockNum(std::string file_name)
{
    Buffer& BufferManager = *database;
    blockInfo* p;
    char* ph;
    int block_num = -1;
    do
 	{
        p = BufferManager.get_file_block(file_name, 1, block_num + 1);
        block_num++;
        ph = BufferManager.get_content(p);
    } while(ph[0] != '\0');
    return block_num;
}

template <class T>
void BPTree<T>::writeBack()
{
    Buffer& BufferManager = *database;
    std::string file_path = ROOT + file_name;
	getFile(file_path);
	int block_num = getBlockNum(file_name);
	
	BPT temp_head;
    int i;
    unsigned j;
	
	for(i = 0, j = 0; temp_head != NULL; i++)
	{
		char* ph = BufferManager.get_content(BufferManager.get_file_block(file_name, 1, i));
		int offset = 0;
		memset(ph, 0, BLOCK_LEN);
		for(j = 0; j < temp_head -> num_of_key; j++)
		{
			ph[offset++] = '*';
			ph[offset++] = ' ';
			copyStr(ph, offset, temp_head -> keys[j]);
			ph[offset++] = ' ';
			copyStr(ph, offset, temp_head -> values[j]);
			ph[offset++] = ' ';
		}
		ph[offset] = '\0';
		BufferManager.set_dirty(BufferManager.get_file_block(file_name, 1, i), true);
		
		temp_head = temp_head -> next;
	}
	
	while(i < block_num)
	{
		char* ph = BufferManager.get_content(BufferManager.get_file_block(file_name, 1, i));
		memset(ph, 0, BLOCK_LEN);
		BufferManager.set_dirty(BufferManager.get_file_block(file_name, 1, i), true);
		i++;
	}
	return;
}

template <class T>
void BPTree<T>::readAllData()
{
    Buffer& BufferManager = *database;
    std::string file_path = ROOT + file_name;
    getFile(file_path);
    int block_num = getBlockNum(file_name);

	if(block_num <= 0) block_num = 1;
	for(int i = 0; i < block_num; i++) 
	{
        char* ph = BufferManager.get_content(BufferManager.get_file_block(file_name, 1, i));
		readFromDisk(ph, ph + BLOCK_LEN);
    }
}

template <class T>
void BPTree<T>::readFromDisk(char* ph, char* end)
{
    T key;
    int value;
    
    for (int i = 0; i < BLOCK_LEN; i++)
        if (ph[i] != '*') return;
        else 
		{
            i += 2;
            char temp[100];
            int j;
            
            for(j = 0; i < BLOCK_LEN && ph[i] != ' '; i++) temp[j++] = ph[i];
            temp[j] = '\0';
            std::string sa(temp);
            std::stringstream streama(sa);
            streama >> key;           
            memset(temp, 0, sizeof(temp));   
            i++;
            
            for(j = 0; i < BLOCK_LEN && ph[i] != ' '; i++) temp[j++] = ph[i];
            temp[j] = '\0';
            std::string sb(temp);
            std::stringstream streamb(sb);
            streamb >> value;
            
            insertKey(key, value);
        }
}

template <class T>
void BPTree<T>::searchInRange(T &keyA, T &keyB, std::vector<int>& values, int flag)
{
	if (!root) return;
	if (flag == 2)
	{
		tempSearch tSA;
		findLeafWithKey(root, keyA, tSA);
		bool finished = false;
		BPT node = tSA.temp_node;
		unsigned int index = tSA.temp_index;

		do 
		{
			finished = node -> findInRange(index, values);
			index = 0;
			if(node -> next == NULL) break;
			else node = node -> nextLeafNode();
		} while (!finished);
	} 
	else if (flag == 1) 
	{
		tempSearch tSB;
		findLeafWithKey(root, keyB, tSB);
		bool finished = false;
		BPT node = tSB.temp_node;
		unsigned int index = tSB.temp_index;

		do 
		{
			finished = node -> findInRange(index, values);
			index = 0;
			if(node -> next == NULL) break;
			else node = node -> nextLeafNode();
		} while (!finished);
	} 
	else 
	{
		tempSearch tSA, tSB;
	    findLeafWithKey(root, keyA, tSA);
		findLeafWithKey(root, keyB, tSB);
		bool finished = false;
		unsigned int index;
		if (keyA <= keyB) 
		{
			BPT node = tSA.temp_node;
			index = tSA.temp_index;
			do
			{
				finished = node -> findBelowKey(index, keyB, values);
				index = 0;
				if(node -> next == NULL) break;
				else node = node -> nextLeafNode();
			} while (!finished);
		} 
		else 
		{
			BPT node = tSB.temp_node;
			index = tSB.temp_index;
			do 
			{
				finished = node -> findBelowKey(index, keyA, values);
				index = 0;
				if(node -> next == NULL) break;
				else node = node -> nextLeafNode();
			} while (!finished);
		}
	}

	std::sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
	return;
}

#endif

