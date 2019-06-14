#include "RecordManager.h"

void RecordManager::createTableFile(std::string tableName) {
	tableName = tableName + ".txt";
	FILE* f = fopen(tableName.c_str(), "w");
	fclose(f);
}

void RecordManager::dropTableFile(std::string tableName) {
	tableName = tableName + ".txt";
	remove(tableName.c_str());
}

void RecordManager::insertRecord(std::string tableName, Tuple& tuple)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	//�����Ƿ����
	if (!cm.hasTable(tmpName)){
		throw tableNotExists();
	}
	TableInfo attr = cm.getTableInfo(tmpName);
	std::vector<data> v = tuple.getData();
	//�������Ԫ��ĸ��������Ƿ�Ϸ�
	for (int i = 0; i < v.size(); i++) {
		if (v[i].type != attr.types[i])
			throw tupleTypeConflict();
	}
	Table table = selectRecord(tmpName);
	std::vector<Tuple>& tuples = table.getTuple();
	//����Ƿ����unique��ͻ
	for (int i = 0; i < attr.unique.size(); i++) {
		if (attr.unique[i] == true) {
			if (isConflict(tuples, v, i) == true)
				throw uniqueConflict();
		}
	}
	//�쳣�ж����
	int blockNum = cm.getBlockNum(tableName);
	if (blockNum <= 0)blockNum = 1;
	char* p = bm.getPage(tableName, blockNum - 1);
	int i;
	for (i = 0; p[i] != '\0'&&i < PAGESIZE; i++);
	int j;
	int len = 0;
	//����Tuple�ĳ���
	for (j = 0; j < v.size(); j++) {
		data d = v[j];
		if(d.type == "int") {
			int t = getDataLength(d.datai);
			len += t; break;
		}
		else if(d.type == "float") {
			float t = getDataLength(d.dataf);
			len += t; break;
		}
		else {
			len += d.datas.length();
		}
	}
	len += v.size() + 7;
	int blockOffset;//���ռ�¼������Ŀ�ı��
	//���ʣ��Ŀռ��㹻�����tuple
	if (PAGESIZE - i >= len) {
		blockOffset = blockNum - 1;
		//�����Ԫ��
		insertRecord1(p, i, len, v);
		//д�ر��ļ�
		int pageId = bm.getPageId(tableName, blockNum - 1);
		bm.modifyPage(pageId);
	}
	//���ʣ��Ŀռ䲻��
	else {
		blockOffset = blockNum;
		//����һ����
		char* p = bm.getPage(tableName, blockNum);
		//�������Ŀ��в����Ԫ��
		insertRecord1(p, 0, len, v);
		//д�ر��ļ�
		int pageId = bm.getPageId(tableName, blockNum);
		bm.modifyPage(pageId);
	}
	//�������е�Index
	std::vector<std::string> table_name;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<IndexInfo> indexinfo = cm.getIndexInfo();
	for (int i = 0; i < indexinfo.size(); i++) {
		table_name[i] = indexinfo[i].tableName;
		attributeNames[i] = indexinfo[i].attributeName;
		types[i] = indexinfo[i].type;
	}
	IndexManager im(table_name, attributeNames, types);
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (cm.attributeHasIndex(tmpName, attr.attributeNames[i]) == true) {
			std::string attr_name = attr.attributeNames[i];
			std::string file_path = "IndexManager\\" + tmpName + "_" + attr.attributeNames[i] + ".txt";
			std::vector<data> d = tuple.getData();
			if (attr.types[i] == "int") {
				std::string key = std::to_string(d[i].datai);
				im.insertIndex(file_path, attr.types[i], key, blockOffset, i);
			}
			else if (attr.types[i] == "float") {
				std::string key = std::to_string(d[i].dataf);
				im.insertIndex(file_path, attr.types[i], key, blockOffset, i);
			}
			else {
				im.insertIndex(file_path, attr.types[i], d[i].datas, blockOffset, i);
			}
		}
	}
}

int RecordManager::deleteRecord(std::string tableName)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	//�����Ƿ����
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	
	int blockNum = cm.getBlockNum(tableName);
	//���ļ���СΪ0ʱֱ�ӷ���
	if(blockNum <= 0)
		return 0;
	TableInfo attr = cm.getTableInfo(tmpName);
	int count = 0;
	//�������е�Index
	std::vector<std::string> table_name;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<IndexInfo> indexinfo = cm.getIndexInfo();
	for (int i = 0; i < indexinfo.size(); i++) {
		table_name[i] = indexinfo[i].tableName;
		attributeNames[i] = indexinfo[i].attributeName;
		types[i] = indexinfo[i].type;
	}
	IndexManager im(table_name, attributeNames, types);
	//�������п�
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName,i);
		char *t = p;
		while (*p != '\0'&&p < t + PAGESIZE) {
			Tuple tuple = readTuple(p, attr);
			for (int j = 0; j < attr.attributeNames.size(); j++) {
				if (cm.attributeHasIndex(tmpName, attr.attributeNames[i]) == true) {
					std::string attr_name = attr.attributeNames[i];
					std::string file_path = "IndexManager\\" + tmpName + "_" + attr.attributeNames[i] + ".txt";
					std::vector<data> d = tuple.getData();
					if (attr.types[i] == "int") {
						std::string key = std::to_string(d[i].datai);
						im.deleteIndexByKey(file_path, attr.types[i], key);
					}
					else if (attr.types[i] == "float") {
						std::string key = std::to_string(d[i].dataf);
						im.deleteIndexByKey(file_path, attr.types[i], key);
					}
					else {
						im.deleteIndexByKey(file_path, attr.types[i], d[i].datas);
					}
				}
			}
			//ɾ����¼
			p = deleteRecord1(p);
			count++;
		}
		int pageId = bm.getPageId(tableName, i);
		bm.modifyPage(pageId);
	}
	return count;
}

int RecordManager::deleteRecord(std::string tableName, std::string target_attr,Where where)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	TableInfo attr = cm.getTableInfo(tmpName);
	int index = -1;
	bool flag = false;//�ж��Ƿ��������
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.attributeNames[i] == target_attr) {
			index = i;
			if (cm.attributeHasIndex(tmpName, target_attr) == true)
				flag = true;
			break;
		}
	}
	if (index == -1) {
		throw attributeNotExists();
	}
	else if (attr.types[index] != where.data.type) {
		throw dataTypeConflict();
	}
	int count = 0;
	if (flag == true && where.relation_character != NOT_EQUAL) {
		//��������ͨ��������ÿ��
		std::vector<int> block_ids;
		//ͨ��������ȡ���������ļ�¼���ڵĿ��
		searchWithIndex(tmpName, target_attr, where, block_ids);
		for (int i = 0; i < block_ids.size(); i++) {
			count += conditionDeleteInBlock(tmpName, block_ids[i], attr, index, where);
		}
	}
	else {
		int blockNum = cm.getBlockNum(tableName);
		if (blockNum <= 0)return 0;
		for (int i = 0; i < blockNum; i++) {
			count += conditionDeleteInBlock(tmpName, i, attr, index, where);
		}
	}
	return count;
}

Table RecordManager::selectRecord(std::string tableName, std::string resultTableName)
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}

	int blockNum = cm.getBlockNum(tableName);
	if(blockNum<=0)blockNum = 1;
	TableInfo attr = cm.getTableInfo(tmpName);
	Table table(resultTableName, attr);
	std::vector<Tuple> &v = table.getTuple();
	//�������п�
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName, i);
		char *t = p;
		//�����������м�¼
		while (*p != '\0' && p < t + PAGESIZE) {
			//��ȡ��¼
			Tuple tuple = readTuple(p, attr);
			//�����¼û�б�ɾ����������ӵ�table��
			if (tuple.isDeleted() == false)
				v.push_back(tuple);
			int len = getTupleLength(p);
			p = p + len;
		}
	}
	return table;
}

Table RecordManager::selectRecord(std::string tableName, std::string target_attr, Where where, std::string result_table_name) {
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	//�����Ƿ����
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	TableInfo attr = cm.getTableInfo(tmpName);
	int index = -1;
	bool flag = false;
	//��ȡĿ�����Եı��
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.attributeNames[i] == target_attr) {
			index = i;
			if (cm.attributeHasIndex(tmpName, target_attr) == true)
				flag = true;
			break;
		}
	}
	//Ŀ�����Բ����ڣ��׳��쳣
	if (index == -1) {
		throw attributeNotExists();
	}
	//where�����е��������ݵ����Ͳ�ƥ�䣬�׳��쳣
	else if (attr.types[index] != where.data.type) {
		throw dataTypeConflict();
	}

	//�쳣������

	//����table
	Table table(result_table_name, attr);
	std::vector<Tuple>& v = table.getTuple();
	if (flag == true && where.relation_character != NOT_EQUAL) {
		std::vector<int> block_ids;
		//ʹ��������ȡ���������ļ�¼���ڿ��
		searchWithIndex(tmpName, target_attr, where, block_ids);
		for (int i = 0; i < block_ids.size(); i++) {
			conditionSelectInBlock(tmpName, block_ids[i], attr, index, where, v);
		}
	}
	else {
		//��ȡ�ļ���ռ�Ŀ������
		int block_num = cm.getBlockNum(tableName);
		//�����ļ���СΪ0���������
		if (block_num <= 0)
			block_num = 1;
		//�������п�
		for (int i = 0; i < block_num; i++) {
			conditionSelectInBlock(tmpName, i, attr, index, where, v);
		}
	}
	return table;
}
//Insert�ĸ�������
void RecordManager::insertRecord1(char* p, int offset, int len, const std::vector<data>& v)
{
	std::stringstream stream;
	stream << len;
	std::string s = stream.str();
	while (s.length() < 4)
		s = "0" + s;
	for (int j = 0; j < s.length(); j++, offset++)
		p[offset] = s[j];
	for (int j = 0; j < v.size(); j++) {
		p[offset] = ' ';
		offset++;
		data d = v[j];
		if (d.type == "int") {
			copyString(p, offset, d.datai);
		}
		else if(d.type == "float") {
			copyString(p, offset, d.dataf);
		}
		else {
			copyString(p, offset, d.datas);
		}
	}
	p[offset] = ' ';
	p[offset + 1] = '0';
	p[offset + 2] = '\n';
}
//Delete�ĸ�������
char* RecordManager::deleteRecord1(char* p) {
	int len = getTupleLength(p);
	p = p + len;
	*(p - 2) = '1';
	return p;
}
//���ڴ��ж�ȡһ��tuple
Tuple RecordManager::readTuple(char* p, TableInfo attr) {
	Tuple tuple;
	p = p + 5;
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		data data;
		data.type = attr.types[i];
		char tmp[100];
		int j;
		for (j = 0; *p != ' '; j++, p++) {
			tmp[j] = *p;
		}
		tmp[j] = '\0';
		p++;
		std::string s(tmp);
		if(data.type == "int") {
			std::stringstream stream(s);
			stream >> data.datai;
		}
		else if(data.type == "float") {
			std::stringstream stream(s);
			stream >> data.dataf;
		}
		else {
			data.datas = s;
		}
		tuple.addData(data);
	}
	if (*p == '1')
		tuple.setDeleted();
	return tuple;
}
//��ȡһ��tuple�ĳ���
int RecordManager::getTupleLength(char* p) {
	char tmp[10];
	int i;
	for (i = 0; p[i] != ' '; i++)
		tmp[i] = p[i];
	tmp[i] = '\0';
	std::string s(tmp);
	int len = stoi(s);
	return len;
}
//�ڿ��н�������ɾ��
int RecordManager::conditionDeleteInBlock(std::string tableName, int block_id, TableInfo attr, int index, Where where) {
	//��ȡ��ǰ��ľ��
	tableName = tableName + ".txt";//����
	char* p = bm.getPage(tableName, block_id);
	char* t = p;
	int count = 0;
	//�����������м�¼
	while (*p != '\0' && p < t + PAGESIZE) {
		//��ȡ��¼
		Tuple tuple = readTuple(p, attr);
		std::vector<data> d = tuple.getData();
		//������������ִ�в�ͬ����
		if(attr.types[index] == "int") {
			//�������where����
			if (isSatisfied(d[index].datai, where.data.datai, where.relation_character) == true) {
				//����¼ɾ��
				p = deleteRecord1(p);
				count++;
			}
			//���������where�����������ü�¼
			else {
				int len = getTupleLength(p);
				p = p + len;
			}
		}
		else if(attr.types[index] == "float"){
			if (isSatisfied(d[index].dataf, where.data.dataf, where.relation_character) == true) {
				p = deleteRecord1(p);
				count++;
			}
			else {
				int len = getTupleLength(p);
				p = p + len;
			}
		}
		else {
			if (isSatisfied(d[index].datas, where.data.datas, where.relation_character) == true) {
				p = deleteRecord1(p);
				count++;
			}
			else {
				int len = getTupleLength(p);
				p = p + len;
			}
		}
	}
	//����ǰ��д���ļ�
	int page_id = bm.getPageId(tableName, block_id);
	bm.modifyPage(page_id);
	return count;
}
//����������
void RecordManager::searchWithIndex(std::string tableName, std::string attributeName, Where where, std::vector<int>& block_ids) {
	std::string file_path = "IndexManager\\" + tableName + "_" + attributeName + ".txt";
	CatalogManager cm;
	//�������е�Index
	std::vector<std::string> table_name;
	std::vector<std::string> attributeNames;
	std::vector<std::string> types;
	std::vector<IndexInfo> indexinfo = cm.getIndexInfo();
	for (int i = 0; i < indexinfo.size(); i++) {
		table_name[i] = indexinfo[i].tableName;
		attributeNames[i] = indexinfo[i].attributeName;
		types[i] = indexinfo[i].type;
	}
	IndexManager im(table_name, attributeNames, types);
	std::vector<std::string> relations;
	switch (where.relation_character) {
	case LESS: {
		relations.push_back("<");
		break;
	}
	case LESS_OR_EQUAL: {
		relations.push_back("<=");
		break;
	}
	case EQUAL: {
		relations.push_back("=");
		break;
	}
	case GREATER: {
		relations.push_back(">");
		break;
	}
	case GREATER_OR_EQUAL: {
		relations.push_back(">=");
		break;
	}
	case NOT_EQUAL: {
		relations.push_back("<>");
		break;
	}
	}
	std::vector<std::string> searchTables;
	searchTables.push_back(file_path);
	std::vector<std::string> searchTypes;
	searchTypes.push_back(where.data.type);
	std::vector<std::string> searchKeys;
	if (where.data.type == "int") 
		searchKeys.push_back(std::to_string(where.data.datai));
	else if(where.data.type == "float")
		searchKeys.push_back(std::to_string(where.data.dataf));
	else searchKeys.push_back(where.data.datas);

	std::vector<Location> Ls = im.searchRange(searchTables, relations, searchTypes, searchKeys);
	for (std::size_t i = 0; i < Ls.size(); i++) {
		std::cout << Ls[i].blockNum << std::endl;
	}

}
//�ڿ��н���������ѯ
void RecordManager::conditionSelectInBlock(std::string table_name, int block_id, TableInfo attr, int index, Where where, std::vector<Tuple>& v) {
	//��ȡ��ǰ��ľ��
	table_name = table_name + ".txt";//����
	char* p = bm.getPage(table_name, block_id);
	char* t = p;
	//�������м�¼
	while (*p != '\0' && p < t + PAGESIZE) {
		//��ȡ��¼
		Tuple tuple = readTuple(p, attr);
		//�����¼�ѱ�ɾ���������ü�¼
		if (tuple.isDeleted() == true) {
			int len = getTupleLength(p);
			p = p + len;
			continue;
		}
		std::vector<data> d = tuple.getData();
		//������������ѡ��
		if (attr.types[index] == "int") {
			//�����������򽫸�Ԫ����ӵ�table
			if (isSatisfied(d[index].datai, where.data.datai, where.relation_character) == true) {
				v.push_back(tuple);
			}
			//�����������������ü�¼
		}
			//ͬcase1
		else if(attr.types[index] == "float"){
			if (isSatisfied(d[index].dataf, where.data.dataf, where.relation_character) == true) {
				v.push_back(tuple);
			}
		}
			//ͬcase1
		else {
			if (isSatisfied(d[index].datas, where.data.datas, where.relation_character) == true) {
				v.push_back(tuple);
			}
		}
		int len = getTupleLength(p);
		p = p + len;
	}
}
//�жϲ���ļ�¼�Ƿ��������¼��ͻ
bool RecordManager::isConflict(std::vector<Tuple>& tuples, std::vector<data>& v, int index) {
	for (int i = 0; i < tuples.size(); i++) {
		if (tuples[i].isDeleted() == true)
			continue;
		std::vector<data> d = tuples[i].getData();
		if (v[index].type == "int") {
			if (v[index].datai == d[index].datai)
				return true;
		}
		else if (v[index].type == "float") {
			if (v[index].dataf == d[index].dataf)
				return true;
		}
		else {
			if (v[index].datas == d[index].datas)
				return true;
		}
		return false;
	}
}
