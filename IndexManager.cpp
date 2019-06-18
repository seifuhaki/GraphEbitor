#include "IndexManager.h"

IndexManager::IndexManager(const std::vector<std::string> tableNames, const std::vector<std::string> attributeNames, const std::vector<std::string> types)
{
	indexIntMap.clear();
	indexFloatMap.clear();
	indexStringMap.clear();

	for (std::size_t i = 0; i < tableNames.size(); i++) {
		createIndex("IndexManager\\" + tableNames[i] + "_" + attributeNames[i] + ".txt", types[i]);
	}

}

IndexManager::~IndexManager()
{
	for (intMap::iterator itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++) {
		if (itInt->second) {
			delete itInt->second;
		}
	}
	for (stringMap::iterator itString = indexStringMap.begin(); itString != indexStringMap.end(); itString++) {
		if (itString->second) {
			delete itString->second;
		}
	}
	for (floatMap::iterator itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++) {
		if (itFloat->second) {
			delete itFloat->second;
		}
	}
}

void IndexManager::createIndex(std::string file_path, std::string type)
{


	if (type == "int") {
		BPlusTree<int> *tree = new BPlusTree<int>(file_path, 32, "int");
		indexIntMap.insert(intMap::value_type(file_path, tree));
	}
	else if (type == "float") {
		BPlusTree<float> *tree = new BPlusTree<float>(file_path, 32, "float");
		indexFloatMap.insert(floatMap::value_type(file_path, tree));
	}
	else {
		int key_size = getKeySize(type) + 8;
		BPlusTree<std::string> *tree = new BPlusTree<std::string>(file_path, key_size, "char");
		indexStringMap.insert(stringMap::value_type(file_path, tree));
	}

	return;
}

void IndexManager::dropIndex(std::string file_path, std::string type)
{
	//根据不同数据类型采用对应的处理方式
	if (type == "int") {
		//查找路径对应的键值对
		intMap::iterator itInt = indexIntMap.find(file_path);
		if (itInt == indexIntMap.end()) { //未找到
			// cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			//删除对应的B+树
			delete itInt->second;
			std::remove(file_path.c_str());
			//清空该键值对
			indexIntMap.erase(itInt);
		}
	}
	else if (type == "float") { //同上
		floatMap::iterator itFloat = indexFloatMap.find(file_path);
		if (itFloat == indexFloatMap.end()) {
			// cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			delete itFloat->second;
			std::remove(file_path.c_str());
			indexFloatMap.erase(itFloat);
		}
	}
	else {
		stringMap::iterator itString = indexStringMap.find(file_path);
		if (itString == indexStringMap.end()) { //同上
			// cout << "Error:in drop index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			delete itString->second;
			std::remove(file_path.c_str());
			indexStringMap.erase(itString);
		}
	}

	return;
}

Location IndexManager::findIndex(std::string file_path, std::string type, std::string key)
{

	if (type == "int") {
		intMap::iterator itInt = indexIntMap.find(file_path);
		if (itInt == indexIntMap.end()) { //未找到
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			throw indexNotExist();
		}
		else
			//找到则返回对应的键值
			return itInt->second->btree_find(itInt->second->roots, atoi(key.c_str()));
	}
	else if (type == "float") {
		floatMap::iterator itFloat = indexFloatMap.find(file_path);
		if (itFloat == indexFloatMap.end()) { //同上
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			throw indexNotExist();
		}
		else
			return itFloat->second->btree_find(itFloat->second->roots, atof(key.c_str()));
	}
	else {
		stringMap::iterator itString = indexStringMap.find(file_path);
		if (itString == indexStringMap.end()) { //同上
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			throw indexNotExist();
		}
		else
			return itString->second->btree_find(itString->second->roots, key);
	}
}

void IndexManager::insertIndex(std::string file_path, std::string type, std::string key, int blockNum, int offset)
{
	//setKey(type, key);

	if (type == "int") {
		intMap::iterator itInt = indexIntMap.find(file_path);
		if (itInt == indexIntMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<int> d;
			d.blockNum = blockNum;
			d.offset = offset;
			d.key = atoi(key.c_str());
			itInt->second->roots = itInt->second->btree_insert(itInt->second->roots, d);
		}

	}
	else if (type == "float") {
		floatMap::iterator itFloat = indexFloatMap.find(file_path);
		if (itFloat == indexFloatMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<float> d;
			d.blockNum = blockNum;
			d.offset = offset;
			d.key = atof(key.c_str());
			itFloat->second->roots = itFloat->second->btree_insert(itFloat->second->roots, d);
		}
	}
	else {
		stringMap::iterator itString = indexStringMap.find(file_path);
		if (itString == indexStringMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<std::string> d;
			d.blockNum = blockNum;
			d.offset = offset;
			d.key = key;
			itString->second->roots = itString->second->btree_insert(itString->second->roots, d);
		}
	}

	return;
}

void IndexManager::deleteIndexByKey(std::string file_path, std::string type, std::string key)
{
	//setKey(type, key);

	if (type == "int") {
		intMap::iterator itInt = indexIntMap.find(file_path);
		if (itInt == indexIntMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<int> d;
			d.blockNum = 0;
			d.offset = 0;
			d.key = atoi(key.c_str());
			itInt->second->roots = itInt->second->btree_delete(itInt->second->roots, d);
		}
	}
	else if (type == "float") {
		floatMap::iterator itFloat = indexFloatMap.find(file_path);
		if (itFloat == indexFloatMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<float> d;
			d.blockNum = 0;
			d.offset = 0;
			d.key = atof(key.c_str());
			itFloat->second->roots = itFloat->second->btree_delete(itFloat->second->roots, d);
		}
	}
	else {
		stringMap::iterator itString = indexStringMap.find(file_path);
		if (itString == indexStringMap.end()) {
			// cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			Data<std::string> d;
			d.blockNum = 0;
			d.offset = 0;
			d.key = key;
			itString->second->roots = itString->second->btree_delete(itString->second->roots, d);
		}
	}

}


int IndexManager::getKeySize(const std::string type)
{
	std::string temp = type.substr(4, 3);
	removeChara(temp, '#');
	return atoi(temp.c_str());
}

std::vector<Location> IndexManager::searchRange(const std::vector<std::string> file_paths, const std::vector<std::string> relations, const std::vector<std::string> types, const std::vector<std::string> keys)
{
	std::vector<Location> results;
	results.clear();
	for (std::size_t i = 0; i < file_paths.size(); i++) {
		std::vector<Location> newResults;
		newResults.clear();
		if (types[i] == "int") {
			intMap::iterator itInt = indexIntMap.find(file_paths[i]);
			if (itInt == indexIntMap.end()) {
				throw indexNotExist();
			}
			else {
				int x = atoi(keys[i].c_str());
				newResults = itInt->second->btree_searchRange(itInt->second->roots, x, relations[i]);
			}

		}
		else if (types[i] == "float") {
			floatMap::iterator itFloat = indexFloatMap.find(file_paths[i]);
			if (itFloat == indexFloatMap.end()) {
				throw indexNotExist();
			}
			else {
				float x = atof(keys[i].c_str());
				newResults = itFloat->second->btree_searchRange(itFloat->second->roots, x, relations[i]);
			}
		}
		else {
			stringMap::iterator itString = indexStringMap.find(file_paths[i]);
			if (itString == indexStringMap.end()) {
				throw indexNotExist();
			}
			else {
				newResults = itString->second->btree_searchRange(itString->second->roots, keys[i], relations[i]);
			}
		}
		if (i == 0) {
			for (std::size_t j = 0; j < newResults.size(); j++) {
				results.push_back(newResults[j]);
			}
		}
		else {
			for (std::size_t j = results.size() - 1; j >= 0 && j < 999999; j--) {
				bool del = true;
				for (std::size_t k = 0; k < newResults.size(); k++) {
					if (results[j].blockNum == newResults[k].blockNum && results[j].offset == newResults[k].offset) {
						del = false;
						break;
					}

				}
				if (del) {
					results.erase(results.begin() + j);
				}
			}
		}
	}
	return results;
}

void IndexManager::removeChara(std::string &str, char c) {
	for (std::size_t i = str.size() - 1; i >= 0 && i < str.size(); i--) {
		if (str.at(i) == c) {
			str.erase(str.begin() + i);
		}
	}
}
