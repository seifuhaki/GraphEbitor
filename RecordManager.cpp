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
		throw(tableNotExists());
	}
	TableInfo attr = cm.getTableInfo(tmpName);
	std::vector<data> v = tuple.getData();
	//�������Ԫ��ĸ��������Ƿ�Ϸ�
	for (int i = 0; i < v.size(); i++) {
		if (v[i].type != attr.types[i])
			throw (tupleTypeConflict());
	}
	Table table = selectRecord(tmpName);
	std::vector<Tuple>& tuples = table.getTuple();
	//����Ƿ����unique��ͻ
	for (int i = 0; i < attr.unique.size(); i++) {
		if (attr.unique[i] == true) {
			if (isConflict(tuples, v, i) == true)
				throw (uniqueConflict());
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
	//��������
	for (int i = 0; i < attr.attributeNames.size(); i++) {
		if (attr.has_index[i] == true) {
			IndexManager im(tmpName, attr.attributeNames[i], attr.types[i]);
			std::string attr_name = attr.attributeNames[i];
			std::string file_path = "INDEX_FILE_" + attr_name + "_" + tmp_name;
			std::vector<data> d = tuple.getData();
			
			index_manager.insertIndex(file_path, d[i], block_offset);
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
	//getAttribute
	//Indexmanager
	int count = 0;
	//�������п�
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName,i);
		char *t = p;
		while (*p != '\0'&&p < t + PAGESIZE) {
			//��������

			//ɾ����¼
			p = deleteRecord1(p);
			count++;
		}
		int pageId = bm.getPageId(tableName, i);
		bm.modifyPage(pageId);
	}
	return count;
}

int RecordManager::deleteRecord(std::string tableName, std::string AttributeName, Where where) 
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	//getAttribute
	int index = -1;
	bool flag = false;//�ж��Ƿ��������

	int count = 0;
	if (flag == true && where.relation_character != NOT_EQUAL) {
		//��������ͨ��������ÿ��
	}
	else {
		int blockNum = cm.getBlockNum(tableName);
		if (blockNum <= 0)return 0;
		for (int i = 0; i < blockNum; i++) {
			//count +=conditionDeleteInBlock();
		}
	}
	return count;
}

Table selectRecord(std::string tableName, std::string resultTableName = "tmptable")
{
	std::string tmpName = tableName;
	tableName = tableName + ".txt";
	CatalogManager cm;
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}

	int blockNum = cm.getBlockNum(tableName);
	if(blockNum<=0)blockNum = 1;
	Attribute attr = cm.getAttribute(tmpName);
	Table table(resultTableName, attr);
	std::vector<Tuple> &v = table.getTuple();
	//�������п�
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName, i);
		char *t = p;
		while (*p != '\0'&& p < t + PAGESIZE) {
			Tuple tuple = readTuple(p, attr);
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
	Attribute attr = cm.getAttribute(tmpName);
	int index = -1;
	bool flag = false;
	//��ȡĿ�����Եı��
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == target_attr) {
			index = i;
			if (attr.has_index[i] == true)
				flag = true;
			break;
		}
	}
	//Ŀ�����Բ����ڣ��׳��쳣
	if (index == -1) {
		throw attribute_not_exist();
	}
	//where�����е��������ݵ����Ͳ�ƥ�䣬�׳��쳣
	else if (attr.type[index] != where.data.type) {
		throw data_type_conflict();
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
		switch (d.type) {
		case -1: {
			copyString(p, offset, d.datai);
		}; break;
		case 0: {
			copyString(p, offset, d.dataf);
		}; break;
		default: {
			copyString(p, offset, d.datas);
		};
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
Tuple RecordManager::readTuple(const char* p, Attribute attr) {
	Tuple tuple;
	p = p + 5;
	for (int i = 0; i < attr.num; i++) {
		data data;
		data.type = attr.type[i];
		char tmp[100];
		int j;
		for (j = 0; *p != ' '; j++, p++) {
			tmp[j] = *p;
		}
		tmp[j] = '\0';
		p++;
		std::string s(tmp);
		switch (data.type) {
		case -1: {
			std::stringstream stream(s);
			stream >> data.datai;
		}; break;
		case 0: {
			std::stringstream stream(s);
			stream >> data.dataf;
		}; break;
		default: {
			data.datas = s;
		}
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
//����������
void RecordManager::searchWithIndex(std::string table_name, std::string target_attr, Where where, std::vector<int>& block_ids) {
	IndexManager index_manager(table_name);
	Data tmp_data;
	std::string file_path = "INDEX_FILE_" + target_attr + "_" + table_name;
	if (where.relation_character == LESS || where.relation_character == LESS_OR_EQUAL) {
		if (where.data.type == -1) {
			tmp_data.type = -1;
			tmp_data.datai = -INF;
		}
		else if (where.data.type == 0) {
			tmp_data.type = 0;
			tmp_data.dataf = -INF;
		}
		else {
			tmp_data.type = 1;
			tmp_data.datas = "";
		}
		index_manager.searchRange(file_path, tmp_data, where.data, block_ids);
	}
	else if (where.relation_character == GREATER || where.relation_character == GREATER_OR_EQUAL) {
		if (where.data.type == -1) {
			tmp_data.type = -1;
			tmp_data.datai = INF;
		}
		else if (where.data.type == 0) {
			tmp_data.type = 0;
			tmp_data.dataf = INF;
		}
		else {
			tmp_data.type = -2;
		}
		index_manager.searchRange(file_path, where.data, tmp_data, block_ids);
	}
	else {
		index_manager.searchRange(file_path, where.data, where.data, block_ids);
	}
}

//�ڿ��н�������ɾ��
int RecordManager::conditionDeleteInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where) {
	//��ȡ��ǰ��ľ��
	table_name = "./database/data/" + table_name;//����
	char* p = buffer_manager.getPage(table_name, block_id);
	char* t = p;
	int count = 0;
	//�����������м�¼
	while (*p != '\0' && p < t + PAGESIZE) {
		//��ȡ��¼
		Tuple tuple = readTuple(p, attr);
		std::vector<Data> d = tuple.getData();
		//������������ִ�в�ͬ����
		switch (attr.type[index]) {
		case -1: {
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
		}; break;
			//ͬcase1
		case 0: {
			if (isSatisfied(d[index].dataf, where.data.dataf, where.relation_character) == true) {
				p = deleteRecord1(p);
				count++;
			}
			else {
				int len = getTupleLength(p);
				p = p + len;
			}
		}; break;
			//ͬcase1
		default: {
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
	}
	//����ǰ��д���ļ�
	int page_id = buffer_manager.getPageId(table_name, block_id);
	// buffer_manager.flushPage(page_id , table_name , block_id);
	// ��Ϊ
	buffer_manager.modifyPage(page_id);
	return count;
}

//�ڿ��н���������ѯ
void RecordManager::conditionSelectInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where, std::vector<Tuple>& v) {
	//��ȡ��ǰ��ľ��
	table_name = "./database/data/" + table_name;//����
	char* p = buffer_manager.getPage(table_name, block_id);
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
		std::vector<Data> d = tuple.getData();
		//������������ѡ��
		switch (attr.type[index]) {
		case -1: {
			//�����������򽫸�Ԫ����ӵ�table
			if (isSatisfied(d[index].datai, where.data.datai, where.relation_character) == true) {
				v.push_back(tuple);
			}
			//�����������������ü�¼
		}; break;
			//ͬcase1
		case 0: {
			if (isSatisfied(d[index].dataf, where.data.dataf, where.relation_character) == true) {
				v.push_back(tuple);
			}
		}; break;
			//ͬcase1
		default: {
			if (isSatisfied(d[index].datas, where.data.datas, where.relation_character) == true) {
				v.push_back(tuple);
			}
		};
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
template <typename T>
int getDataLength(T data) {
	std::stringstream stream;
	stream << data;
	return stream.str().length();
}
template <typename T>
void copyString(char* p, int& offset, T data) {
	std::stringstream stream;
	stream << data;
	std::string s1 = stream.str();
	for (int i = 0; i < s1.length(); i++, offset++)
		p[offset] = s1[i];
}


