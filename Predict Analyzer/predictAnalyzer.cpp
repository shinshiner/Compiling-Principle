#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <set>

using namespace std;

ifstream in;
ofstream out;

//保存产生式的信息
class generator {
public:
	string left;		//产生式左部
	set<string> right;	//产生式右部
	generator(string s){ 
		left = s; 
	}
	void print() {
		out << left << "->";
		set<string>::iterator it = right.begin();
		if (right.begin() != right.end()) {
			out << (*it);
			it++;
		}
		for (; it != right.end(); it++) {
			out << '|' << (*it);
		}
		out << '\n';
	}
	void insert(string s) { 
		right.insert(s); 
	}
};

map<string, set<string>> first;		//FIRST集合
map<string, set<string>> follow;	//FOLLOW集合
map<string, int> VN_dic;			//非终结符索引
set<string> signals;				//所有符号
set<string> VT;						//终结符集
set<string> VN;						//非终结符集
vector<generator> Gener;			//产生式集
vector<map<string, string> > predict;		//预测分析表
int n;

const int MAXLEN = 100;
bool flag[MAXLEN];			//求FIRST集合用

//读入文本文件
void readin() {
	int i, j, k, tmpright;
	string tmpstr, vn, vn_right, rubbish, tmpsig;
	char ch[MAXLEN];

	in >> n;
	in.get();

	for (i = 0; i < n; ++i) {
		vn.clear();
		in.getline(ch, 80, '\n');
		tmpstr = ch;

		//读产生式左部
		j = 0;
		while (tmpstr[j] != ' ') {
			vn.push_back(tmpstr[j]);
			j++;
		}
		if (!VN_dic[vn]) {
			Gener.push_back(vn);
			VN_dic[vn] = Gener.size();
		}
		if (!VN.count(vn)) {
			VN.insert(vn);
		}
		//读产生式右部
		j += 4;
		tmpright = j;
		while (j < tmpstr.size()) {
			while (j < tmpstr.size() && tmpstr[j] != ' ') {
				tmpsig.push_back(tmpstr[j]);
				j++;
			}
			if (!signals.count(tmpsig)) {
				signals.insert(tmpsig);
			}
			tmpsig.clear();
			if (j + 1 < tmpstr.size() && tmpstr[j + 1] != '|') {
				j++;
			}
			else {
				for (k = tmpright; k < j; ++k) {
					vn_right.push_back(tmpstr[k]);
				}
				int tmp = VN_dic[vn] - 1;		//记录非终结符顺序
				Gener[tmp].insert(vn_right);
				vn_right.clear();

				j += 3;
				tmpright = j;
			}			
		}
	}

	//终结符集
	for (set<string>::iterator it = signals.begin(); it != signals.end(); it++) {
		if (!VN.count(*it)) {
			VT.insert(*it);
		}
	}
	//这里把$也算在终结符集里面
	VT.insert("$");
}

//消除直接左递归
void delLeftRec() {
	for (int i = 0; i < Gener.size(); ++i) {
		string str = Gener[i].left;
		set<string>* temp = &(Gener[i].right), *temp1 = NULL;
		set<string>::iterator it = temp->begin();
		string tt = Gener[i].left;
		bool flag = false;

		while (!flag) {
			flag = true;
			for (set<string>::iterator it = signals.begin(); it != signals.end(); it++) {
				if ((*it) == tt) {
					tt = tt + "'";
					flag = false;
				}
			}
		}

		flag = true;
		for (; it != temp->end(); it++) {
			string now;	//待搜索字符
			int j;
			for (j = 0; j < it->length() && (*it)[j] != ' '; ++j) {
				now.push_back((*it)[j]);
			}
			if (now == str) {	//搜索到直接左递归
				Gener.push_back(generator(tt));
				temp1 = &(Gener[i].right);
				VN_dic[tt] = Gener.size();
				flag = false;
				break;
			}
		}
		int x = VN_dic[tt] - 1;
		if (flag) {
			continue;
		}
		vector<string> cont;
		set<string>& ss = Gener[x].right;		//新产生式右部
		ss.insert("~");			//新产生式插入空串
		VN.insert(tt);
		if (signals.count("~") == 0) {
			signals.insert("~");
			VT.insert("~");
		}
		while (!temp1->empty()) {		//处理原产生式右部
			string now;	//待搜索字符
			int j;
			set<string>::iterator it1 = temp1->begin();
			for (j = 0; j < it1->length() && (*it1)[j] != ' '; ++j) {
				now.push_back((*it1)[j]);
			}
			if (now == str) {
				ss.insert(temp1->begin()->substr(now.size()+1) + " " + tt);
			}
			else {
				cont.push_back(temp1->begin()->substr(0) + " " + tt);
			}
			temp1->erase(temp1->begin());		//原产生式右部清理
		}
		for (int i = 0; i < cont.size(); ++i) {
			temp1->insert(cont[i]);
		}
	}

	//输出消除结果
	/*for (int i = 0; i < Gener.size(); ++i) {
		Gener[i].print();
	}*/
}

//求解FIRST集合
void solveFirst(int x) {
	if (flag[x]) {
		return;
	}
	flag[x] = 1;

	string& left = Gener[x].left;
	set<string>& right = Gener[x].right;
	set<string>::iterator it = right.begin();

	for (; it != right.end(); it++) {
		bool flag = true;
		for (int i = 0; i < it->length();) {
			string now;	//待搜索字符
			int j;
			for (j = i; j < it->length() && (*it)[j] != ' '; ++j) {
				now.push_back(it->at(j));
				i++;
			}
			if (VT.count(now)) { //为终结符，直接加入FIRST集合
				first[left].insert(now);
				break;
			}
			else if (VN.count(now)) {	//非终结符，“遍历”其产生式右部
				int y;
				if (VN_dic.count(it->substr(i - now.size(), now.size()))) {
					y = VN_dic[it->substr(i - now.size(), now.size())] - 1;
				}
				i++;	//现在i指向产生式右部下一个符号

				string& tleft = Gener[y].left;
				solveFirst(y);
				set<string>& temp = first[tleft];
				set<string>::iterator it1 = temp.begin();
				bool tmp_flag = true;
				for (; it1 != temp.end(); it1++) {
					if ((*it1) == "~") {		//处理空串
						tmp_flag = false;
					}
					first[left].insert(*it1);
				}
				if (tmp_flag) {
					break;
				}
			}
			else {
				i++;
			}
		}
	}
}

//求解FIRST集合
void getFIRST() {
	int i;

	//本过程实际上是一个深搜
	for (i = 0; i < MAXLEN; ++i) {
		flag[i] = 0;
	}
	for (i = 0; i < Gener.size(); ++i) {
		solveFirst(i);
	}

	//输出FIRST集合
	/*out << "FIRST集合\n";
	map<string, set<string> >::iterator it = first.begin();
	set<string>::iterator it2 = signals.begin();
	for (; it != first.end(); it++)
	{
		out << "FIRST(" << it->first << ")={";
		set<string> & temp = it->second;
		set<string>::iterator it1 = temp.begin();
		bool tmp_flag = false;
		for (; it1 != temp.end(); it1++) {
			if (tmp_flag) {
				out << ",";
			}
			out << (*it1);
			tmp_flag = true;
		}
		out << "}\n";
	}*/
}

//将非终结符str1的FOLLOW集合加到非终结符str2的FOLLOW集合
void append(const string& str1, const string& str2) {
	set<string>& come = follow[str1];
	set<string>& go = follow[str2];
	set<string>::iterator it = come.begin();
	for (; it != come.end(); it++) {
		go.insert(*it);
	}
}

//求解FOLLOW集合
void getFOLLOW() {
	follow[Gener[0].left].insert("$");		//在开始符的FOLLOW集合加入$符号
	while (true){
		bool goon = false;
		for (int i = 0; i < Gener.size(); ++i) {
			string& left = Gener[i].left;
			set<string>& right = Gener[i].right;
			set<string>::iterator it = right.begin();
			for (; it != right.end(); it++) {
				bool flag = true;
				const string& str = *it;
				for (int j = it->length() - 1; j >= 0;) {
					string now;	//待搜索字符
					int jj;
					for (jj = j; jj >=0 && (*it)[jj] != ' '; --jj) {
						now.push_back(it->at(jj));
						j--;
					}
					j++;		//现在j指向(*it)中now的第一个字符
					reverse(now.begin(),now.end());

					//非终结符
					if (VN.count(now)) {
						int x = VN_dic[it->substr(j, now.size())] - 1;
						if (flag) {
							int tt = follow[it->substr(j, now.size())].size();
							append(left, it->substr(j, now.size()));	//FOLLOW集合的交互
							if (!Gener[x].right.count("~")) {
								flag = false;
							}
							int tt1 = follow[it->substr(j, now.size())].size();
							if (tt1 > tt) {
								goon = true;
							}
						}
						for (int k = j + now.size() + 1; k < it->length();) {
							string now1;	//待搜索字符
							int kk;
							for (kk = k; kk < it->length() && (*it)[kk] != ' '; ++kk) {
								now1.push_back(it->at(kk));
								k++;
							}

							if (VN.count(now1)) {	//非终结符
								string id;
								id = it->substr(k - now1.size(), now1.size());
								set<string>& from = first[id];
								set<string>& to = follow[it->substr(j, now.size())];
								set<string>::iterator it1 = from.begin();
								int tt = follow[it->substr(j, now.size())].size();
								//FIRST集合中除空字外的符号都加入FOLLOW(B)
								for (; it1 != from.end(); it1++) {
									if ((*it1) != "~") {
										to.insert(*it1);
									}
								}
								int tt1 = follow[it->substr(j, now.size())].size();
								if (tt1 > tt) {
									goon = true;
								}
								if (!Gener[VN_dic[id] - 1].right.count("~")) {
									break;
								}
							}
							else if(VT.count(now1)) {	//终结符
								int tt = follow[it->substr(j, now.size())].size();
								string tmpstr = now1;
								follow[it->substr(j, now.size())].insert(tmpstr);
								int tt1 = follow[it->substr(j, now.size())].size();
								if (tt1 > tt) {
									goon = true;
								}
								break;
							}
						}
						j -= 2;
					}
					else {
						j -= 2;
						flag = false;
					}
				}
			}
		}
		if (!goon) break;
	}

	//输出FOLLOW集合
	/*out << "FOLLOW集合\n";
	map<string, set<string> >::iterator it = follow.begin();
	for (; it != follow.end(); it++) {
		out << "FOLLOW(" << it->first << ")={";
		set<string> & temp = it->second;
		set<string>::iterator it1 = temp.begin();
		bool flag = false;
		for (; it1 != temp.end(); it1++) {
			if (flag) {
				out << ",";
			}
			out << (*it1);
			flag = true;
		}
		out << "}\n";
	}*/
}

//检查str是否属于text的FIRST集合
bool searchFIRST(const string& text, string str) {
	for (int i = 0; i < text.length();) {
		bool hasEmpty = false;
		string now;	//待搜索字符
		int j;
		for (j = i; j < text.length() && text[j] != ' '; ++j) {
			now.push_back(text[j]);
			i++;
		}
		i++;
		if (VT.count(now)) {	//终结符的FIRST集合就是其本身
			if (now != str) {
				return false;
			}
			else {
				return true;
			}
		}
		else if (VN.count(now)) {
			set<string>& dic = first[now];
			set<string>::iterator it = dic.begin();
			for (; it != dic.end(); it++) {
				if (*it == "~") {
					hasEmpty = true;
				}
				if (*it == str) {
					return true;
				}
			}
			if (!hasEmpty) {
				break;
			}
		}
		else {
			continue;
		}
	}
	return false;
}

//检查str是否属于text的FOLLOW集合
bool searchFOLLOW(const string& text, string str) {
	set<string>& dic = follow[text];
	set<string>::iterator it = dic.begin();
	for (; it != dic.end(); it++) {
		if (*it == str) {
			return true;
		}
	}
	return false;
}

//构造预测分析表
void getTable() {
	map<string, string> temp;
	vector<string> letter;

	for (int i = 0; i < Gener.size(); ++i) {	//实现书上描述的构造逻辑
		temp.clear();
		string& left = Gener[i].left;
		set<string>& right = Gener[i].right;
		set<string>::iterator it = right.begin();
		for (; it != right.end(); it++) {
			for (set<string>::iterator it1 = VT.begin(); it1 != VT.end(); it1++) {
				if (searchFIRST(*it, *it1)) {
					temp[*it1] = *it;
				}
				if (it->at(0) == '~' && searchFOLLOW(left, *it1)) {
					temp[*it1] = *it;
				}
				if (searchFIRST(*it, "~") && searchFOLLOW(left, "$")) {
					temp["$"] = *it;
				}
			}
		}
		predict.push_back(temp);
	}

	//输出预测分析表
	bool tmpflag = false;
	for (set<string>::iterator it = VT.begin(); it != VT.end(); it++) {
		if ((*it) == "~") {
			tmpflag = true;
		}
	}
	if (!tmpflag) {			//这一步仅仅是为了输出好看。。
		VT.insert("~");
	}
	for (int i = 0; i <= VT.size() * 12; ++i) {
		out << "-";
	}
	out << '\n';
	out << "|          |";
	for (set<string>::iterator it = VT.begin(); it != VT.end(); it++) {
		if ((*it) == "~") {
			continue;
		}
		out.setf(ios::left);
		out << setw(11) << *it << "|";
	}
	out << '\n';
	for (int i = 0; i <= VT.size() * 12; ++i) {
		out << "-";
	}
	out << '\n';
	for (int i = 0; i < Gener.size(); ++i) {
		out.setf(ios::left);
		out << setw(11) << Gener[i].left << "|";
		for (set<string>::iterator it = VT.begin(); it != VT.end(); it++) {
			if ((*it) == "~") {
				continue;
			}
			if (predict[i].count(*it)) {
				string tmp;
				tmp = Gener[i].left + "->" + predict[i][*it];
				out.setf(ios::left);
				out << setw(11) << tmp << "|";
			}
			else {
				out << "           |";
			}
		}
		out << '\n';
		for (int i = 0; i <= VT.size() * 12; ++i) {
			out << "-";
		}
		out << '\n';
	}
}

//以下函数为编程时debug用，在实际程序中不需要运行
/*
void printGener() {
	for (int i = 0; i < Gener.size(); ++i) {
		Gener[i].print();
	}
}

void printVN_dic() {
	for (map<string,int>::iterator i = VN_dic.begin(); i != VN_dic.end(); ++i) {
		out << (*i).first << '\n';
	}
}

void printVT() {
	for (set<string>::iterator i = VT.begin(); i != VT.end(); ++i) {
		out << (*i) << '\n';
	}
}
*/

int main()
{
	in.open("test.txt");
	out.open("result.txt");

	readin();		//读入数据
	delLeftRec();	//消除左递归
	getFIRST();		//求FIRST集合
	getFOLLOW();	//求FOLLOW集合
	getTable();		//求预测分析表

	in.close();
	out.close();

    return 0;
}
