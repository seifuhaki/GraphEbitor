#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include <cstdio>
#include <string>
#include "BufferManager.h"
#include <cstring>
#include "Exception.h"
#include <sstream>


#define M 128

class Location {
	public:
		int blockNum;
		int offset;
};

template <typename T>
class Data {
	public:
		T key;
		int blockNum;
		int offset;
};

template <typename T>
class btree_node {
public:
	Data<T> k[2 * M - 1];
	btree_node<T> *p[2 * M];
	int num;
	bool is_leaf;
	btree_node<T> *prev;
	btree_node<T> *next;
};


template <typename T>
class BPlusTree {
public:
	int degree;
	int keySize;
	btree_node<T> *roots;
	FILE *pfile;
	std::string fileName;
	std::string type;
	BufferManager bm;
	int btree_node_num;

public:
	btree_node<T>* btree_create();
	btree_node<T>* btree_node_new();
	int btree_split_child(btree_node<T> *parent, int pos, btree_node<T> *child);
	void btree_insert_nonfull(btree_node<T> *node, Data<T> target);
	void btree_merge_child(btree_node<T> *root, int pos, btree_node<T> *y, btree_node<T> *z);
	void btree_delete_nonone(btree_node<T> *root, Data<T> target);
	void btree_shift_to_left_child(btree_node<T> *root, int pos, btree_node<T> *y, btree_node<T> *z);
	void btree_shift_to_right_child(btree_node<T> *root, int pos, btree_node<T> *y, btree_node<T> *z);
	btree_node<T> *btree_insert(btree_node<T> *root, Data<T> target);
	btree_node<T> *btree_delete(btree_node<T> *root, Data<T> target);
	void removeChara(std::string &str, char c);
	std::string addStr(const std::string &str, std::size_t length);
	Location btree_find(btree_node<T> *root, T key);
	std::vector<Location> btree_searchRange(btree_node<T> *root, T key, std::string relation);

public:
	BPlusTree(std::string fileName, int keySize, std::string type);
	~BPlusTree();
	void initTree();
	int getBlockNum(const std::string fileName);
	void writtenbackToDiskAll();
};


template <typename T>
btree_node<T>* BPlusTree<T>::btree_node_new()
{
	btree_node<T> *node = new btree_node<T>;
	if (NULL == node) {
		return NULL;
	}

	for (int i = 0; i < 2 * this->degree - 1; i++) {
		//node->k[i] = 0;
	}

	for (int i = 0; i < 2 * this->degree; i++) {
		node->p[i] = NULL;
	}

	node->num = 0;
	node->is_leaf = true;
	node->prev = NULL;
	node->next = NULL;
	return node;
}

template <typename T>
btree_node<T> *BPlusTree<T>::btree_create()
{
	btree_node<T> *node = btree_node_new();
	if (NULL == node) {
		return NULL;
	}

	node->next = node;
	node->prev = node;

	return node;
}

template <typename T>
int BPlusTree<T>::btree_split_child(btree_node<T> *parent, int pos, btree_node<T> *child)
{
	btree_node<T> *new_child = btree_node_new();
	if (NULL == new_child) {
		return -1;
	}

	new_child->is_leaf = child->is_leaf;
	new_child->num = this->degree - 1;

	for (int i = 0; i < this->degree - 1; i++) {
		new_child->k[i] = child->k[i + this->degree];
	}

	if (false == new_child->is_leaf) {
		for (int i = 0; i < this->degree; i++) {
			new_child->p[i] = child->p[i + this->degree];
		}
	}

	child->num = this->degree - 1;
	if (true == child->is_leaf) {
		child->num++;
	}

	for (int i = parent->num; i > pos; i--) {
		parent->p[i + 1] = parent->p[i];
	}
	parent->p[pos + 1] = new_child;

	for (int i = parent->num - 1; i >= pos; i--) {
		parent->k[i + 1] = parent->k[i];
	}
	parent->k[pos] = child->k[this->degree - 1];

	parent->num += 1;

	// update link
	if (true == child->is_leaf) {
		new_child->next = child->next;
		child->next->prev = new_child;
		new_child->prev = child;
		child->next = new_child;
	}
	return 1;
}

template <typename T>
void BPlusTree<T>::btree_insert_nonfull(btree_node<T> *node, Data<T> target)
{
	if (true == node->is_leaf) {
		int pos = node->num;
		while (pos >= 1 && target.key < node->k[pos - 1].key) {
			node->k[pos] = node->k[pos - 1];
			pos--;
		}

		node->k[pos] = target;
		node->num += 1;
		btree_node_num += 1;

	}
	else {
		int pos = node->num;
		while (pos > 0 && target.key < node->k[pos - 1].key) {
			pos--;
		}

		if (2 * this->degree - 1 == node->p[pos]->num) {
			btree_split_child(node, pos, node->p[pos]);
			if (target.key > node->k[pos].key) {
				pos++;
			}
		}

		btree_insert_nonfull(node->p[pos], target);
	}
}

template <typename T>
btree_node<T>* BPlusTree<T>::btree_insert(btree_node<T> *root, Data<T> target)
{
	if (NULL == root) {
		return NULL;
	}

	if (2 * this->degree - 1 == root->num) {
		btree_node<T> *node = btree_node_new();
		if (NULL == node) {
			return root;
		}

		node->is_leaf = false;
		node->p[0] = root;
		btree_split_child(node, 0, root);
		btree_insert_nonfull(node, target);
		return node;
	}
	else {
		btree_insert_nonfull(root, target);
		return root;
	}
}

template <typename T>
void BPlusTree<T>::btree_merge_child(btree_node<T> *root, int pos, btree_node<T> *y, btree_node<T> *z)
{
	if (true == y->is_leaf) {
		y->num = 2 * this->degree - 2;
		for (int i = this->degree; i < 2 * this->degree - 1; i++) {
			y->k[i - 1] = z->k[i - this->degree];
		}
	}
	else {
		y->num = 2 * this->degree - 1;
		for (int i = this->degree; i < 2 * this->degree - 1; i++) {
			y->k[i] = z->k[i - this->degree];
		}
		y->k[this->degree - 1] = root->k[pos];
		for (int i = this->degree; i < 2 * this->degree; i++) {
			y->p[i] = z->p[i - this->degree];
		}
	}

	for (int j = pos + 1; j < root->num; j++) {
		root->k[j - 1] = root->k[j];
		root->p[j] = root->p[j + 1];
	}

	root->num -= 1;

	// update link
	if (true == y->is_leaf) {
		y->next = z->next;
		z->next->prev = y;
	}

	free(z);
}

template <typename T>
btree_node<T> *BPlusTree<T>::btree_delete(btree_node<T> *root, Data<T> target)
{
	if (1 == root->num) {
		btree_node<T> *y = root->p[0];
		btree_node<T> *z = root->p[1];
		if (NULL != y && NULL != z &&
			this->degree - 1 == y->num && this->degree - 1 == z->num) {
			btree_merge_child(root, 0, y, z);
			free(root);
			btree_delete_nonone(y, target);
			return y;
		}
		else {
			btree_delete_nonone(root, target);
			return root;
		}
	}
	else {
		btree_delete_nonone(root, target);
		return root;
	}
}

template <typename T>
void BPlusTree<T>::btree_delete_nonone(btree_node<T> *root, Data<T> target)
{
	if (true == root->is_leaf) {
		int i = 0;
		while (i < root->num && target.key > root->k[i].key) i++;
		if (target.key == root->k[i].key) {
			for (int j = i + 1; j < 2 * this->degree - 1; j++) {
				root->k[j - 1] = root->k[j];
			}
			root->num -= 1;
			btree_node_num -= 1;

		}
		else {
			printf("target not found\n");
		}
	}
	else {
		int i = 0;
		btree_node<T> *y = NULL, *z = NULL;
		while (i < root->num && target.key > root->k[i].key) i++;

		y = root->p[i];
		if (i < root->num) {
			z = root->p[i + 1];
		}
		btree_node<T> *p = NULL;
		if (i > 0) {
			p = root->p[i - 1];
		}

		if (y->num == this->degree - 1) {
			if (i > 0 && p->num > this->degree - 1) {
				btree_shift_to_right_child(root, i - 1, p, y);
			}
			else if (i < root->num && z->num > this->degree - 1) {
				btree_shift_to_left_child(root, i, y, z);
			}
			else if (i > 0) {
				btree_merge_child(root, i - 1, p, y);
				y = p;
			}
			else {
				btree_merge_child(root, i, y, z);
			}
			btree_delete_nonone(y, target);
		}
		else {
			btree_delete_nonone(y, target);
		}
	}
}


template <typename T>
void BPlusTree<T>::btree_shift_to_right_child(btree_node<T> *root, int pos,
	btree_node<T> *y, btree_node<T> *z)
{
	z->num += 1;

	if (false == z->is_leaf) {
		z->k[0] = root->k[pos];
		root->k[pos] = y->k[y->num - 1];
	}
	else {
		z->k[0] = y->k[y->num - 1];
		root->k[pos] = y->k[y->num - 2];
	}

	for (int i = z->num - 1; i > 0; i--) {
		z->k[i] = z->k[i - 1];
	}

	if (false == z->is_leaf) {
		for (int i = z->num; i > 0; i--) {
			z->p[i] = z->p[i - 1];
		}
		z->p[0] = y->p[y->num];
	}

	y->num -= 1;
}

template <typename T>
void BPlusTree<T>::btree_shift_to_left_child(btree_node<T> *root, int pos,
	btree_node<T> *y, btree_node<T> *z)
{
	y->num += 1;

	if (false == z->is_leaf) {
		y->k[y->num - 1] = root->k[pos];
		root->k[pos] = z->k[0];
	}
	else {
		y->k[y->num - 1] = z->k[0];
		root->k[pos] = z->k[0];
	}

	for (int j = 1; j < z->num; j++) {
		z->k[j - 1] = z->k[j];
	}

	if (false == z->is_leaf) {
		y->p[y->num] = z->p[0];
		for (int j = 1; j <= z->num; j++) {
			z->p[j - 1] = z->p[j];
		}
	}

	z->num -= 1;
}

template <typename T>
BPlusTree<T>::BPlusTree(std::string fileName, int keySize, std::string type)
{
	this->degree = PAGESIZE / keySize / 2;
	this->keySize = keySize;
	this->fileName = fileName;
	this->type = type;
	this->btree_node_num = 0;
	if (fopen_s(&this->pfile, fileName.c_str(), "r"))
	{
		fopen_s(&this->pfile, fileName.c_str(), "w");
		this->roots = btree_create();
		fclose(this->pfile);
	}
	else
	{
		fclose(this->pfile);
		this->roots = btree_create();
		initTree();
	}
}

template <typename T>
void BPlusTree<T>::initTree()
{
	Data<T> newData;
	int blockNum = getBlockNum(this->fileName);
	for (int i = 0; i < blockNum; i++) {
		char *buf = bm.getPage(this->fileName, i);
		std::string temp(buf);
		for (int j = 0; j < 2 * this->degree - 1; j++) {
			if (temp.size() < keySize*(j + 1)) {
				break;
			}
			std::string t = temp.substr(j*this->keySize, this->keySize - 8);
			std::stringstream stream(t);
			stream >> newData.key;
			t = temp.substr((j+1)*this->keySize-8, 4);
			removeChara(t, '#');
			newData.blockNum = atoi(t.c_str());
			t = temp.substr((j + 1)*this->keySize - 4, 4);
			removeChara(t, '#');
			newData.offset = atoi(t.c_str());
			this->roots = btree_insert(this->roots, newData);
			
		}

	}
}

template <typename T>
BPlusTree<T>::~BPlusTree()
{
	writtenbackToDiskAll();
}

template <typename T>
int BPlusTree<T>::getBlockNum(const std::string fileName) {
	char* p;
	int block_num = -1;
	do {
		p = bm.getPage(fileName, block_num + 1);
		block_num++;
	} while (p[0] != '\0');
	return block_num;
}

template <typename T>
void BPlusTree<T>::removeChara(std::string &str, char c) {
	for (std::size_t i = str.size() - 1; i >= 0 && i < str.size(); i--) {
		if (str.at(i) == c) {
			str.erase(str.begin() + i);
		}
	}
}

template <typename T>
void BPlusTree<T>::writtenbackToDiskAll() {
	std::string info = "";
	int blockNum = getBlockNum(this->fileName);
	int cnt = 0;
	if (NULL != this->roots) {
		btree_node<T> *root = this->roots;
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}

		btree_node<T> *iter = leftmost;

		do {
			info.clear();
			char * buf = bm.getPage(this->fileName, cnt);
			int pageId = bm.getPageId(this->fileName, cnt);
			for (int i = 0; i < iter->num; i++) {
				std::ostringstream os;
				os.clear();
				os << iter->k[i].key;
				std::string t = addStr(os.str(), this->keySize - 8);
				info += t;
				info += addStr(std::to_string(iter->k[i].blockNum), 4);
				info += addStr(std::to_string(iter->k[i].offset), 4);
				//std::cout << info << std::endl;
			}
			strncpy_s(buf, PAGESIZE, info.c_str(), strlen(info.c_str()));
			bm.modifyPage(pageId);
			cnt++;
			iter = iter->next;
		} while (iter != leftmost);
	}
	for (int i = cnt; i <= blockNum; i++) {
		char * buf = bm.getPage(this->fileName, i);
		for (int j = 0; j < PAGESIZE; j++) {
			buf[j] = 0;
		}
		bm.modifyPage(i);
	}
}

template <typename T>
std::string BPlusTree<T>::addStr(const std::string &str, std::size_t length) {
	if (str.length() > length) {
		throw nameTooLong();
	}
	std::string temp = "";
	temp += str;
	for (std::size_t i = temp.length(); i < length; i++) {
		temp += '#';
	}

	return temp;
}

template <typename T>
Location BPlusTree<T>::btree_find(btree_node<T> *root, T key) {
	if (true == root->is_leaf) {
		int i = 0;
		while (i < root->num && key > root->k[i].key) i++;
		if (key == root->k[i].key) {
			Location L;
			L.blockNum = root->k[i].blockNum;
			L.offset = root->k[i].offset;
			return L;
		}
		else {
			throw targetNotFound();
		}
	}
	else {
		int i = 0;
		while (i < root->num && key > root->k[i].key) i++;
		return btree_find(root->p[i], key);
	}
}

template <typename T>
std::vector<Location> BPlusTree<T>::btree_searchRange(btree_node<T> *root, T key, std::string relation) {
	std::vector<Location> results;
	results.clear();
	if (relation == "<") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key < key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
					continue;
				}
				ok = true;
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	else if (relation == ">") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key > key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
				}
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	else if (relation == "<=") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key <= key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
					continue;
				}
				ok = true;
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	else if (relation == ">=") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key >= key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
				}
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	else if (relation == "=") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key == key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
					ok = true;
					break;
				}
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	else if (relation == "<>") {
		btree_node<T> *leftmost = root;
		while (false == leftmost->is_leaf) {
			leftmost = leftmost->p[0];
		}
		btree_node<T> *iter = leftmost;
		int cnt = 0;
		do {
			bool ok = false;
			for (int i = 0; i < iter->num; i++) {
				if (iter->k[i].key != key) {
					Location L;
					L.blockNum = iter->k[i].blockNum;
					L.offset = iter->k[i].offset;
					results.push_back(L);
					ok = true;
					break;
				}
			}
			if (ok) {
				break;
			}
			iter = iter->next;
		} while (iter != leftmost);
		return results;
	}
	throw illegalIdentifier();
}
#endif














