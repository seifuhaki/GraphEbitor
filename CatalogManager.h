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

// ��API��ʹ��ʱֻ��Ҫ���º���
//	createTable, dropTable, createIndex, dropIndex
//	����ͬ���������ж��߼���������ģ����ʵ�֣���������
//	ʱ�ᶪ����Ӧ�Ĵ������ͣ�������º��exception.h��
//	��Ҫ��API��ʵ�ֵ��ж�ֻ��һ�㣬����primaryKey�Ƿ���attributeNames��
//	��Ϊ��һ���Ҫֱ���������ļ��������Ͳ�����CatalogManager������ 
//	���к������Ѿ������ԣ�������Ӧ��û�����⣬��Ҳ���ų�����û�п��ǵ����������
//	��������API���в��Ե�ʱ������������CatalogManager����Ĵ��󣬺���֨һ��������debugһ�� 
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

