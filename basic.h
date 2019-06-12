#ifndef _BASIC_H_
#define _BASIC_H_ 1
#include <iostream>
#include <vector>
#include <string>
//����where���ж� �ֱ�ΪС�ڣ�С�ڵ��ڣ����ڣ����ڵ��ڣ����ڣ�������
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} WHERE;

//һ��struct�������һ����Ϣ�����ͺ�ֵ
//��һ��struntǶ��һ��unionʵ�ֶ������͵�ת��
//type�����ͣ�-1��int,0:float,1-255:string(��ֵΪ�ַ����ĳ���+1),ע��ʹ��ʱ��Value��ѡ��
struct Data {
	int type;
	int datai;
	float dataf;
	std::string datas;
};

//Where���һ���ж����
struct Where {
	Data data; //����
	WHERE relation_character;   //��ϵ
};

//��ȷ������ʱ������str.size()+1������str��type��ֵ��һ�ű����32��attribute
struct Attribute {
	int num;  //���table��������
	std::string name[32];  //���ÿ�����Ե�����
	short type[32];  //���ÿ�����Ե����ͣ�-1��int,0:float,1~255:string�ĳ���+1
	bool unique[32];  //�ж�ÿ�������Ƿ�unique����Ϊtrue
	short primary_key;  //�ж��Ƿ��������,-1Ϊ�����ڣ�������Ϊ����������λ��
	bool has_index[32]; //�ж��Ƿ��������
};

//��������һ�ű����ֻ����10��index
struct Index {
	int num;  //index������
	short location[10];  //ÿ��index��Attribute��name�������ǵڼ���
	std::string indexname[10];  //ÿ��index������
};

//Ԫ���������ֻ������
class Tuple {
private:
	std::vector<Data> data_;  //�洢Ԫ�����ÿ�����ݵ���Ϣ
	bool isDeleted_;
public:
	Tuple() : isDeleted_(false) {};
	Tuple(const Tuple &tuple_in);  //����Ԫ��
	void addData(Data data_in);  //����Ԫ��
	std::vector<Data> getData() const;  //��������
	int getSize();  //����Ԫ�����������
	bool isDeleted();
	void setDeleted();
	void showTuple();  //��ʾԪ���е���������
};

class Table {
private:
	std::string title_;  //����
	std::vector<Tuple> tuple_;  //������е�Ԫ��
	Index index_;  //���������Ϣ
public:
	Attribute attr_;  //���ݵ�����
	//���캯��
	Table() {};
	Table(std::string title, Attribute attr);
	Table(const Table &table_in);

	// int DataSize();  //ÿ��tupleռ�����ݴ�С

	int setIndex(short index, std::string index_name);  //��������������Ҫ����������Attribute�ı�ţ��Լ����������֣��ɹ�����1ʧ�ܷ���0
	int dropIndex(std::string index_name);  //ɾ�����������뽨�������������֣��ɹ�����1ʧ�ܷ���0

	//private������ӿ�
	std::string getTitle();
	Attribute getAttr();
	std::vector<Tuple>& getTuple();
	Index getIndex();
	short gethasKey();

	void showTable(); //��ʾtable�Ĳ�������
	void showTable(int limit);
};


#endif