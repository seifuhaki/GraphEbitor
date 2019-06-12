#ifndef _BASIC_H_
#define _BASIC_H_ 1
#include <iostream>
#include <vector>
#include <string>
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} WHERE;
struct data {
	std::string type;
	int datai;
	float dataf;
	std::string datas;
};
struct Where {
	data data; //����
	WHERE relation_character;   //��ϵ
};
class IndexInfo {
public:
	std::string indexName;
	std::string tableName;
	std::string attributeName;
};
class TableInfo {
public:
	std::string tableName;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<bool> unique;
};
class Tuple {
private:
	std::vector<data> data_;  //�洢Ԫ�����ÿ�����ݵ���Ϣ
	bool isDeleted_;
public:
	Tuple() : isDeleted_(false) {};
	Tuple(const Tuple &tuple_in);  //����Ԫ��
	void addData(data data_in);  //����Ԫ��
	std::vector<data> getData() const;  //��������
	int getSize();  //����Ԫ�����������
	bool isDeleted();
	void setDeleted();
	void showTuple();  //��ʾԪ���е���������
};
class Table {
private:
	std::string tableName;  //����
	std::vector<Tuple> tuple_;  //������е�Ԫ��
	std::vector<IndexInfo> index_;  //���������Ϣ
public:
	TableInfo attr_;  //���ݵ�����
	//���캯��
	Table() {};
	Table(std::string title, TableInfo attr);
	Table(const Table &table_in);

	// int DataSize();  //ÿ��tupleռ�����ݴ�С

	int setIndex(std::string attributeName, std::string index_name);  //��������������Ҫ����������Attribute�ı�ţ��Լ����������֣��ɹ�����1ʧ�ܷ���0
	int dropIndex(std::string index_name);  //ɾ�����������뽨�������������֣��ɹ�����1ʧ�ܷ���0

	//private������ӿ�
	std::string getTitle();
	TableInfo getAttr();
	std::vector<Tuple>& getTuple();
	std::vector<IndexInfo>& getIndex();

	void showTable(); //��ʾtable�Ĳ�������
	void showTable(int limit);
};
#endif