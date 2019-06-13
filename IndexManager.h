#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

#include "BPlusTree.h"

#include <map>
#include <string>
#include "basic.h"
#include "BPlusTree.h"
#include "BufferManager.h"

/*
	��ʼ��IndexManager��Ҫ���빹�캯��������Ҫ�Ĳ������ⲿ���ҵ�ʱ����CatalogManager���油�䣻

	�ڽ��в����Լ�ɾ������ʱ������Index���ֶ���Ҫͨ��InsertIndex��deleteIndexʵʱ����ά���� ���ﲻ����Ҫ��ֵ��
	����ҪRecordManager�ṩ�Ķ�Ӧ��¼�Ŀ�ź�ƫ������

	���еĲ��ҹ��ܶ�����ͨ��searchRange����ʵ�֣����ҷ�ʽ�������д��룺

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

	���ص�Location�����������еĿ�ż�ƫ������Ϣ

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