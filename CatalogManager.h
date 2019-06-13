#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include <string>
#include <vector>
#include "basic.h"
#include "BufferManager.h"
#include "Exception.h"

// fixed length record

#define tableInfoLength 1024
#define maxAttributeNum 16
#define maxIndexNum 7
#define tableNameLength 32
#define attributeNameLength 32
#define indexNameLength 32

#define TableInfoPath "CatalogInfo/TableInfo.txt"
#define IndexInfoPath "CatalogInfo/IndexInfo.txt"

class CatalogManager {
	public:
		CatalogManager();
		void createTable(const std::string tableName, const std::vector<std::string>& attributeNames, const std::vector<std::string>& types, const std::vector<bool>& unique, const std::string primaryKey);
		void dropTable(const std::string tableName);
		bool hasTable(const std::string tableName);
		bool hasAttribute(const std::string tableName, const std::string attributeName);
		void createIndex(const std::string tableName, const std::string attributeName, const std::string indexName);
		bool attributeHasIndex(const std::string tableName, const std::string attributeName);
		void dropIndex(const std::string indexName);
		int getBlockNum(const std::string fileName);
		std::string addStr(const std::string &str, std::size_t length);
		std::string num2str(int num, std::size_t length);
		void removeChara(std::string &str, char c);
		bool hasIndex(const std::string indexName);
		bool isUnique(const std::string tableName, const std::string attributeName);
		std::vector<IndexInfo> getIndexInfo();
		TableInfo getTableInfo(const std::string tableName);
		std::string getType(const std::string tableName, const std::string attributeName);
		

	private:
		BufferManager bm;
		std::string fileName = "TableInfo.txt";

};

#endif // !_CATALOGMANAGER_H_

