#ifndef _API_H_
#define _API_H_ 1

//API�ӿڡ���ΪInterpreter������͵ײ�Manager���ӵĽӿ�
//������������й��ܽӿ�
//APIֻ�������ĳ������жϣ�������쳣�׳����ɸ��ײ�Manager���
class API {
public:
	//���캯��
	API();
	//��������
	~API();
	//���룺������Where������������Where����ֵ��
	//�����void
	//���ܣ����ذ�������Ŀ����������Where�����ļ�¼�ı�
	//�ڶ�������ѯ����£�����Where�µ��߼���������Table��ƴ��
	//�쳣���ɵײ㴦��
	//��������ڣ��׳�table_not_exist�쳣
	//������Բ����ڣ��׳�attribute_not_exist�쳣
	//���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣
	void selectRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Where> where, char operation);
	//���룺������Where������������Where����ֵ��
	//�����void
	//���ܣ�ɾ����Ӧ�����µ�Table�ڼ�¼(��ɾ�����ļ�)
	//�쳣���ɵײ㴦��
	//��������ڣ��׳�table_not_exist�쳣
	//������Բ����ڣ��׳�attribute_not_exist�쳣
	//���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣
	void deleteRecord(std::string tableName);
	void deleteRecord(std::string tableName, std::string attributeName, std::string relation, std::string value);
	//���룺������һ��Ԫ�����
	//�����void
	//���ܣ����Ӧ���ڲ���һ����¼
	//�쳣���ɵײ㴦��
	//���Ԫ�����Ͳ�ƥ�䣬�׳�tuple_type_conflict�쳣
	//���������ͻ���׳�primary_key_conflict�쳣
	//���unique���Գ�ͻ���׳�unique_conflict�쳣
	//��������ڣ��׳�table_not_exist�쳣
	void insertRecord(std::string table_name, Tuple& tuple);
	//���룺���������ԣ�����������
	//������Ƿ񴴽��ɹ�
	//���ܣ������ݿ��в���һ�����Ԫ��Ϣ
	//�쳣���ɵײ㴦��
	//����Ѿ�����ͬ�����ı���ڣ����׳�table_exist�쳣
	void createTable(std::string table_name, Attribute attribute, int primary, Index index);
	//���룺����
	//������Ƿ�ɾ���ɹ�
	//���ܣ������ݿ���ɾ��һ�����Ԫ��Ϣ�����������м�¼(ɾ�����ļ�)
	//�쳣���ɵײ㴦��
	//��������ڣ��׳�table_not_exist�쳣
	void dropTable(std::string table_name);
	//���룺��������������������
	//������Ƿ񴴽��ɹ�
	//���ܣ������ݿ��и��¶�Ӧ���������Ϣ����ָ�������Ͻ���һ��������
	//�쳣���ɵײ㴦��
	//��������ڣ��׳�table_not_exist�쳣
	//�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
	//�����Ӧ�����Ѿ������������׳�index_exist�쳣
	void createIndex(std::string table_name, std::string index_name, std::string attr_name);
	//���룺������������
	//������Ƿ�ɾ���ɹ�
	//���ܣ�ɾ����Ӧ��Ķ�Ӧ�����ϵ�����
	//�쳣���ɵײ㴦��
	//��������ڣ��׳�table_not_exist�쳣
	//�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
	//�����Ӧ����û���������׳�index_not_exist�쳣
	bool dropIndex(std::string table_name, std::string index_name);

private:
	RecordManager record;
	CatalogManager catalog;
};

#endif