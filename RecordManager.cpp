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
	//检测表是否存在
	if (!cm.hasTable(tmpName)){
		throw(tableNotExists());
	}
	TableInfo attr = cm.getTableInfo(tmpName);
	std::vector<data> v = tuple.getData();
	//检测插入的元组的各个属性是否合法
	for (int i = 0; i < v.size(); i++) {
		if (v[i].type != attr.types[i])
			throw (tupleTypeConflict());
	}
	Table table = selectRecord(tmpName);
	std::vector<Tuple>& tuples = table.getTuple();
	//检测是否存在unique冲突
	for (int i = 0; i < attr.unique.size(); i++) {
		if (attr.unique[i] == true) {
			if (isConflict(tuples, v, i) == true)
				throw (uniqueConflict());
		}
	}
	//异常判断完成
	int blockNum = cm.getBlockNum(tableName);
	if (blockNum <= 0)blockNum = 1;
	char* p = bm.getPage(tableName, blockNum - 1);
	int i;
	for (i = 0; p[i] != '\0'&&i < PAGESIZE; i++);
	int j;
	int len = 0;
	//计算Tuple的长度
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
	int blockOffset;//最终记录所插入的块的编号
	//如果剩余的空间足够插入该tuple
	if (PAGESIZE - i >= len) {
		blockOffset = blockNum - 1;
		//插入该元组
		insertRecord1(p, i, len, v);
		//写回表文件
		int pageId = bm.getPageId(tableName, blockNum - 1);
		bm.modifyPage(pageId);
	}
	//如果剩余的空间不够
	else {
		blockOffset = blockNum;
		//新增一个块
		char* p = bm.getPage(tableName, blockNum);
		//在新增的块中插入该元组
		insertRecord1(p, 0, len, v);
		//写回表文件
		int pageId = bm.getPageId(tableName, blockNum);
		bm.modifyPage(pageId);
	}
	//更新索引
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
	//检测表是否存在
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	
	int blockNum = cm.getBlockNum(tableName);
	//表文件大小为0时直接返回
	if(blockNum <= 0)
		return 0;
	//getAttribute
	//Indexmanager
	int count = 0;
	//遍历所有块
	for (int i = 0; i < blockNum; i++) {
		char *p = bm.getPage(tableName,i);
		char *t = p;
		while (*p != '\0'&&p < t + PAGESIZE) {
			//更新索引

			//删除记录
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
	bool flag = false;//判断是否存在索引

	int count = 0;
	if (flag == true && where.relation_character != NOT_EQUAL) {
		//有索引，通过索引获得块号
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
	//遍历所有块
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
	//检测表是否存在
	if (!cm.hasTable(tmpName)) {
		throw tableNotExists();
	}
	Attribute attr = cm.getAttribute(tmpName);
	int index = -1;
	bool flag = false;
	//获取目标属性的编号
	for (int i = 0; i < attr.num; i++) {
		if (attr.name[i] == target_attr) {
			index = i;
			if (attr.has_index[i] == true)
				flag = true;
			break;
		}
	}
	//目标属性不存在，抛出异常
	if (index == -1) {
		throw attribute_not_exist();
	}
	//where条件中的两个数据的类型不匹配，抛出异常
	else if (attr.type[index] != where.data.type) {
		throw data_type_conflict();
	}

	//异常检测完成

	//构建table
	Table table(result_table_name, attr);
	std::vector<Tuple>& v = table.getTuple();
	if (flag == true && where.relation_character != NOT_EQUAL) {
		std::vector<int> block_ids;
		//使用索引获取满足条件的记录所在块号
		searchWithIndex(tmpName, target_attr, where, block_ids);
		for (int i = 0; i < block_ids.size(); i++) {
			conditionSelectInBlock(tmpName, block_ids[i], attr, index, where, v);
		}
	}
	else {
		//获取文件所占的块的数量
		int block_num = cm.getBlockNum(tableName);
		//处理文件大小为0的特殊情况
		if (block_num <= 0)
			block_num = 1;
		//遍历所有块
		for (int i = 0; i < block_num; i++) {
			conditionSelectInBlock(tmpName, i, attr, index, where, v);
		}
	}
	return table;
}
//Insert的辅助函数
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
//Delete的辅助函数
char* RecordManager::deleteRecord1(char* p) {
	int len = getTupleLength(p);
	p = p + len;
	*(p - 2) = '1';
	return p;
}
//从内存中读取一个tuple
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

//获取一个tuple的长度
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
//带索引查找
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

//在块中进行条件删除
int RecordManager::conditionDeleteInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where) {
	//获取当前块的句柄
	table_name = "./database/data/" + table_name;//新增
	char* p = buffer_manager.getPage(table_name, block_id);
	char* t = p;
	int count = 0;
	//遍历块中所有记录
	while (*p != '\0' && p < t + PAGESIZE) {
		//读取记录
		Tuple tuple = readTuple(p, attr);
		std::vector<Data> d = tuple.getData();
		//根据属性类型执行不同操作
		switch (attr.type[index]) {
		case -1: {
			//如果满足where条件
			if (isSatisfied(d[index].datai, where.data.datai, where.relation_character) == true) {
				//将记录删除
				p = deleteRecord1(p);
				count++;
			}
			//如果不满足where条件，跳过该记录
			else {
				int len = getTupleLength(p);
				p = p + len;
			}
		}; break;
			//同case1
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
			//同case1
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
	//将当前块写回文件
	int page_id = buffer_manager.getPageId(table_name, block_id);
	// buffer_manager.flushPage(page_id , table_name , block_id);
	// 改为
	buffer_manager.modifyPage(page_id);
	return count;
}

//在块中进行条件查询
void RecordManager::conditionSelectInBlock(std::string table_name, int block_id, Attribute attr, int index, Where where, std::vector<Tuple>& v) {
	//获取当前块的句柄
	table_name = "./database/data/" + table_name;//新增
	char* p = buffer_manager.getPage(table_name, block_id);
	char* t = p;
	//遍历所有记录
	while (*p != '\0' && p < t + PAGESIZE) {
		//读取记录
		Tuple tuple = readTuple(p, attr);
		//如果记录已被删除，跳过该记录
		if (tuple.isDeleted() == true) {
			int len = getTupleLength(p);
			p = p + len;
			continue;
		}
		std::vector<Data> d = tuple.getData();
		//根据属性类型选择
		switch (attr.type[index]) {
		case -1: {
			//满足条件，则将该元组添加到table
			if (isSatisfied(d[index].datai, where.data.datai, where.relation_character) == true) {
				v.push_back(tuple);
			}
			//不满足条件，跳过该记录
		}; break;
			//同case1
		case 0: {
			if (isSatisfied(d[index].dataf, where.data.dataf, where.relation_character) == true) {
				v.push_back(tuple);
			}
		}; break;
			//同case1
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
//判断插入的记录是否和其他记录冲突
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


