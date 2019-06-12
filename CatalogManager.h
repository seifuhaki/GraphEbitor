#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include <string>
#include <vector>
#include "buffer_manager.h"
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

// 在API中使用时只需要以下函数
//	createTable, dropTable, createIndex, dropIndex
//	作用同函数名，判定逻辑基本已在模块中实现，发生错误
//	时会丢出相应的错误类型（详见更新后的exception.h）
//	需要在API中实现的判定只有一点，就是primaryKey是否在attributeNames中
//	因为这一项不需要直接与数据文件交互，就不放在CatalogManager里面了 
//	所有函数都已经过测试，大体上应该没有问题，但也不排除有我没有考虑到的特殊情况
//	如果在设计API进行测试的时候遇到了属于CatalogManager里面的错误，和我吱一声，我再debug一下 
class CatalogManager {
	public:
		CatalogManager();
		void createTable(const std::string tableName, const std::vector<std::string>& attributeNames, const std::vector<std::string>& types, const std::vector<bool>& unique, const std::string primaryKey);
		void dropTable(const std::string tableName);
		bool hasTable(const std::string tableName);
		bool hasAttribute(const std::string tableName, const std::string attributeName);
		void createIndex(const std::string tableName, const std::string attributeName, const std::string indexName);
		bool attributeHasIndex(const std::string tableName, const std::string attributeName, const std::string indexName);
		void dropIndex(const std::string indexName);
		int getBlockNum(const std::string fileName);
		std::string addStr(const std::string &str, std::size_t length);
		std::string num2str(int num, std::size_t length);
		void removeChara(std::string &str, char c);
		bool hasIndex(const std::string indexName);
	private:
		BufferManager bm;

};

#endif // !_CATALOGMANAGER_H_

