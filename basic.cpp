#include "basic.h"
Tuple::Tuple(const Tuple &tuple_in) {
	for (int index = 0; index < tuple_in.data_.size(); index++)
	{
		this->data_.push_back(tuple_in.data_[index]);
	}
}

inline int Tuple::getSize() {
	return (int)data_.size();
}

//��������
void Tuple::addData(data data_in) {
	this->data_.push_back(data_in);
}

bool Tuple::isDeleted() {
	return isDeleted_;
}

void Tuple::setDeleted() {
	isDeleted_ = true;
}

//�õ�Ԫ���е�����
std::vector<data> Tuple::getData() const {
	return this->data_;
}

void Tuple::showTuple() {
	for (int index = 0; index < getSize(); index++) {
		if (data_[index].type == "int")
			std::cout << data_[index].datai << '\t';
		else if (data_[index].type == "float")
			std::cout << data_[index].dataf << '\t';
		else
			std::cout << data_[index].datas << '\t';
	}
	std::cout << std::endl;
}

//table���캯��
Table::Table(std::string title, TableInfo attr) {
	this->tableName = title;
	this->attr_ = attr;
}

//table�Ĺ��캯����������
Table::Table(const Table &table_in) {
	this->attr_ = table_in.attr_;
	this->tableName = table_in.tableName;
	for (int index = 0; index < tuple_.size(); index++)
		this->tuple_.push_back(table_in.tuple_[index]);
	for (int index = 0; index < index_.size(); index++)
		this->index_.push_back(table_in.index_[index]);
}

//��������
int Table::setIndex(std::string attributeName, std::string indexName) {
	short tmpIndex;
	for (tmpIndex = 0; tmpIndex < index_.size(); tmpIndex++) {
		if (attributeName == index_[tmpIndex].attributeName)  //����Ԫ���Ѿ�������ʱ������
		{
			std::cout << "Illegal Set Index: The index has been in the table." << std::endl;
			return 0;
		}
	}
	for (tmpIndex = 0; tmpIndex < index_.size(); tmpIndex++) {
		if (indexName == index_[tmpIndex].indexName)  //����Ԫ���Ѿ�������ʱ������
		{
			std::cout << "Illegal Set Index: The name has been used." << std::endl;
			return 0;
		}
	}
	IndexInfo INDEX;
	INDEX.attributeName = attributeName;
	INDEX.indexName = indexName;
	INDEX.tableName = this->tableName;
	index_.push_back(INDEX);
	return 1;
}

int Table::dropIndex(std::string indexName) {
	int tmpIndex;
	for (tmpIndex = 0; tmpIndex < index_.size(); tmpIndex++) {
		if (indexName == index_[tmpIndex].indexName) { //����Ԫ���Ѿ�������ʱ������
			index_.erase(index_.begin()+ tmpIndex);
			break;
		}
	}
	if (tmpIndex == index_.size())
	{
		std::cout << "Illegal Drop Index: No such a index in the table." << std::endl;
		return 0;
	}
	return 1;
}


//����һЩprivate��ֵ
std::string Table::getTitle() {
	return tableName;
}
TableInfo Table::getAttr() {
	return attr_;
}
std::vector<Tuple>& Table::getTuple() {
	return tuple_;
}
std::vector<IndexInfo>& Table::getIndex() {
	return index_;
}


void Table::showTable() {
	for (int index = 0; index < attr_.attributeNames.size(); index++)
		std::cout << attr_.attributeNames[index] << '\t';
	std::cout << std::endl;
	for (int index = 0; index < tuple_.size(); index++)
		tuple_[index].showTuple();
}

void Table::showTable(int limit) {
	for (int index = 0; index < attr_.attributeNames.size(); index++)
		std::cout << attr_.attributeNames[index] << '\t';
	std::cout << std::endl;
	for (int index = 0; index < limit&&index < tuple_.size(); index++)
		tuple_[index].showTuple();
}