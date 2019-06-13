#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

#include "BPlusTree.h"

#include <map>
#include <string>
#include "basic.h"
#include "BPlusTree.h"
#include "BufferManager.h"

/*
	初始化IndexManager需要传入构造函数所有需要的参数，这部分我到时候在CatalogManager里面补充；

	在进行插入以及删除操作时对于有Index的字段需要通过InsertIndex和deleteIndex实时进行维护， 这里不仅需要键值，
	还需要RecordManager提供的对应记录的块号和偏移量；

	所有的查找功能都可以通过searchRange函数实现，查找方式参照下列代码：

		relations.push_back("<");
		relations.push_back(">");

		searchTables.push_back("IndexManager\\T1_A1.txt");
		searchTables.push_back("IndexManager\\T1_A1.txt");

		searchTypes.push_back("int");
		searchTypes.push_back("int");

		searchKeys.push_back("10");
		searchKeys.push_back("5");

		std::vector<Location> Ls  = im.searchRange(searchTables, relations, searchTypes, searchKeys);
		for (std::size_t i = 0; i < Ls.size(); i++) {
			std::cout << Ls[i].blockNum << std::endl;
		}

	返回的Location向量包含所有的块号及偏移量信息

*/

class IndexManager {
	public:
		IndexManager(const std::vector<std::string> tableNames, const std::vector<std::string> attributeNames, const std::vector<std::string> types);

		~IndexManager();
		void createIndex(std::string file_path, std::string type);

		void dropIndex(std::string file_path, std::string type);

		Location findIndex(std::string file_path, std::string type, std::string key);
		void insertIndex(std::string file_path, std::string type, std::string key, int blockNum, int offset);

		void deleteIndexByKey(std::string file_path, std::string type, std::string key);
		void searchRange(std::string file_path, data data1, data data2, std::vector<int>& vals);
		std::vector<Location> searchRange(const std::vector<std::string> file_paths, const std::vector<std::string> relations, const std::vector<std::string> types, const std::vector<std::string> keys);
		void removeChara(std::string &str, char c);

private:
		typedef std::map<std::string, BPlusTree<int> *> intMap;
		typedef std::map<std::string, BPlusTree<std::string> *> stringMap;
		typedef std::map<std::string, BPlusTree<float> *> floatMap;

		int static const TYPE_FLOAT = 0;
		int static const TYPE_INT = -1;

		intMap indexIntMap;
		stringMap indexStringMap;
		floatMap indexFloatMap;

		int getKeySize(const std::string type);

};




#endif