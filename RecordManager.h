#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_ 1
#define INF 1000000
#define MAXFRAMESIZE 100
#define PAGESIZE 4096

#include <cstdio> 
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "basic.h"
#include "IndexManager.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "exception.h"
extern BufferManager bm;
class RecordManager {
public:
	//���룺����
	//�����void
	//���ܣ��������ļ�
	//�쳣�����쳣������catalog manager����
	void createTableFile(std::string tableName);
	//���룺����
	//�����void
	//���ܣ�ɾ�����ļ�
	//�쳣�����쳣������catalog manager����
	void dropTableFile(std::string table_name);
	//���룺������һ��Ԫ��
	//�����void
	//���ܣ����Ӧ���в���һ����¼
	//�쳣�����Ԫ�����Ͳ�ƥ�䣬�׳�tuple_type_conflict�쳣�����
	//������ͻ���׳�primary_key_conflict�쳣�����unique���Գ�ͻ��
	//�׳�unique_conflict�쳣����������ڣ��׳�table_not_exist�쳣��
	void insertRecord(std::string tableName, Tuple& tuple);
	//���룺����
	//�����int(ɾ���ļ�¼��)
	//���ܣ�ɾ����Ӧ�������м�¼����ɾ�����ļ���
	//�쳣����������ڣ��׳�table_not_exist�쳣
	int deleteRecord(std::string tableName);
	//���룺������Ŀ�����ԣ�һ��Where���͵Ķ���
	//�����int(ɾ���ļ�¼��)
	//���ܣ�ɾ����Ӧ��������Ŀ������ֵ����Where�����ļ�¼
	//�쳣����������ڣ��׳�table_not_exist�쳣��������Բ����ڣ��׳�attribute_not_exist�쳣��
	//���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣��
	int deleteRecord(std::string tableName, std::string AttributeName, Where where);
	//���룺����
	//�����Table���Ͷ���
	//���ܣ��������ű�
	//�쳣����������ڣ��׳�table_not_exist�쳣
	Table selectRecord(std::string table_name, std::string result_table_name = "tmp_table");
	//���룺������Ŀ�����ԣ�һ��Where���͵Ķ���
	//�����Table���Ͷ���
	//���ܣ����ذ�������Ŀ����������Where�����ļ�¼�ı�
	//�쳣����������ڣ��׳�table_not_exist�쳣��������Բ����ڣ��׳�attribute_not_exist�쳣��
	//���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣��
    Table selectRecord(std::string table_name, std::string target_attr, Where where, std::string result_table_name = "tmp_table");
	//���룺������Ŀ��������
	//�����void
	//���ܣ��Ա����Ѿ����ڵļ�¼��������
	//�쳣����������ڣ��׳�table_not_exist�쳣��������Բ����ڣ��׳�attribute_not_exist�쳣��
	//void createIndex(IndexManager& index_manager, std::string table_name, std::string target_attr);
private:
	//insertRecord�ĸ�������
	void insertRecord1(char* p, int offset, int len, const std::vector<data>& v);
	//deleteRecord�ĸ�������
	char* deleteRecord1(char* p);
	//���ڴ��ж�ȡһ��tuple
	Tuple readTuple(const char* p, Attribute attr);
	//��ȡһ��tuple�ĳ���
	int getTupleLength(char* p);
	//�жϲ���ļ�¼�Ƿ��������¼��ͻ
	bool isConflict(std::vector<Tuple>& tuples, std::vector<data>& v, int index);
	//����������
	void searchWithIndex(std::string table_name, std::string target_attr, Where where, std::vector<int>& block_ids);
	//�ڿ��н�������ɾ��
	int conditionDeleteInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where);
	//�ڿ��н���������ѯ
	void conditionSelectInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where, std::vector<Tuple>& v);

	BufferManager bm;
};
#endif