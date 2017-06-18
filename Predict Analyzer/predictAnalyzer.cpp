//515030910468 Ҷ����
/*
    ������ʵ���˴��ı��ж���һ������ֱ����ݹ���������޹��ķ�������Ԥ�������
���ı��ļ���ʽ���
    ʵ��˼�뼰��ز��Կɲμ�readme.txt�ļ�
*/

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

//�������ʽ����Ϣ
class generator {
public:
	string left;		//����ʽ��
	set<string> right;	//����ʽ�Ҳ�
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

map<string, set<string>> first;		//FIRST����
map<string, set<string>> follow;	//FOLLOW����
map<string, int> VN_dic;			//���ս������
set<string> signals;				//���з���
set<string> VT;						//�ս����
set<string> VN;						//���ս����
vector<generator> Gener;			//����ʽ��
vector<map<string, string> > predict;		//Ԥ�������
int n;

const int MAXLEN = 100;
bool flag[MAXLEN];			//��FIRST������

//�����ı��ļ�
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

		//������ʽ��
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
		//������ʽ�Ҳ�
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
				int tmp = VN_dic[vn] - 1;		//��¼���ս��˳��
				Gener[tmp].insert(vn_right);
				vn_right.clear();

				j += 3;
				tmpright = j;
			}			
		}
	}

	//�ս����
	for (set<string>::iterator it = signals.begin(); it != signals.end(); it++) {
		if (!VN.count(*it)) {
			VT.insert(*it);
		}
	}
	//�����$Ҳ�����ս��������
	VT.insert("$");
}

//����ֱ����ݹ�
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
			string now;	//�������ַ�
			int j;
			for (j = 0; j < it->length() && (*it)[j] != ' '; ++j) {
				now.push_back((*it)[j]);
			}
			if (now == str) {	//������ֱ����ݹ�
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
		set<string>& ss = Gener[x].right;		//�²���ʽ�Ҳ�
		ss.insert("~");			//�²���ʽ����մ�
		VN.insert(tt);
		if (signals.count("~") == 0) {
			signals.insert("~");
			VT.insert("~");
		}
		while (!temp1->empty()) {		//����ԭ����ʽ�Ҳ�
			string now;	//�������ַ�
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
			temp1->erase(temp1->begin());		//ԭ����ʽ�Ҳ�����
		}
		for (int i = 0; i < cont.size(); ++i) {
			temp1->insert(cont[i]);
		}
	}

	//����������
	/*for (int i = 0; i < Gener.size(); ++i) {
		Gener[i].print();
	}*/
}

//���FIRST����
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
			string now;	//�������ַ�
			int j;
			for (j = i; j < it->length() && (*it)[j] != ' '; ++j) {
				now.push_back(it->at(j));
				i++;
			}
			if (VT.count(now)) { //Ϊ�ս����ֱ�Ӽ���FIRST����
				first[left].insert(now);
				break;
			}
			else if (VN.count(now)) {	//���ս�����������������ʽ�Ҳ�
				int y;
				if (VN_dic.count(it->substr(i - now.size(), now.size()))) {
					y = VN_dic[it->substr(i - now.size(), now.size())] - 1;
				}
				i++;	//����iָ�����ʽ�Ҳ���һ������

				string& tleft = Gener[y].left;
				solveFirst(y);
				set<string>& temp = first[tleft];
				set<string>::iterator it1 = temp.begin();
				bool tmp_flag = true;
				for (; it1 != temp.end(); it1++) {
					if ((*it1) == "~") {		//����մ�
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

//���FIRST����
void getFIRST() {
	int i;

	//������ʵ������һ������
	for (i = 0; i < MAXLEN; ++i) {
		flag[i] = 0;
	}
	for (i = 0; i < Gener.size(); ++i) {
		solveFirst(i);
	}

	//���FIRST����
	/*out << "FIRST����\n";
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

//�����ս��str1��FOLLOW���ϼӵ����ս��str2��FOLLOW����
void append(const string& str1, const string& str2) {
	set<string>& come = follow[str1];
	set<string>& go = follow[str2];
	set<string>::iterator it = come.begin();
	for (; it != come.end(); it++) {
		go.insert(*it);
	}
}

//���FOLLOW����
void getFOLLOW() {
	follow[Gener[0].left].insert("$");		//�ڿ�ʼ����FOLLOW���ϼ���$����
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
					string now;	//�������ַ�
					int jj;
					for (jj = j; jj >=0 && (*it)[jj] != ' '; --jj) {
						now.push_back(it->at(jj));
						j--;
					}
					j++;		//����jָ��(*it)��now�ĵ�һ���ַ�
					reverse(now.begin(),now.end());

					//���ս��
					if (VN.count(now)) {
						int x = VN_dic[it->substr(j, now.size())] - 1;
						if (flag) {
							int tt = follow[it->substr(j, now.size())].size();
							append(left, it->substr(j, now.size()));	//FOLLOW���ϵĽ���
							if (!Gener[x].right.count("~")) {
								flag = false;
							}
							int tt1 = follow[it->substr(j, now.size())].size();
							if (tt1 > tt) {
								goon = true;
							}
						}
						for (int k = j + now.size() + 1; k < it->length();) {
							string now1;	//�������ַ�
							int kk;
							for (kk = k; kk < it->length() && (*it)[kk] != ' '; ++kk) {
								now1.push_back(it->at(kk));
								k++;
							}

							if (VN.count(now1)) {	//���ս��
								string id;
								id = it->substr(k - now1.size(), now1.size());
								set<string>& from = first[id];
								set<string>& to = follow[it->substr(j, now.size())];
								set<string>::iterator it1 = from.begin();
								int tt = follow[it->substr(j, now.size())].size();
								//FIRST�����г�������ķ��Ŷ�����FOLLOW(B)
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
							else if(VT.count(now1)) {	//�ս��
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

	//���FOLLOW����
	/*out << "FOLLOW����\n";
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

//���str�Ƿ�����text��FIRST����
bool searchFIRST(const string& text, string str) {
	for (int i = 0; i < text.length();) {
		bool hasEmpty = false;
		string now;	//�������ַ�
		int j;
		for (j = i; j < text.length() && text[j] != ' '; ++j) {
			now.push_back(text[j]);
			i++;
		}
		i++;
		if (VT.count(now)) {	//�ս����FIRST���Ͼ����䱾��
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

//���str�Ƿ�����text��FOLLOW����
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

//����Ԥ�������
void getTable() {
	map<string, string> temp;
	vector<string> letter;

	for (int i = 0; i < Gener.size(); ++i) {	//ʵ�����������Ĺ����߼�
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

	//���Ԥ�������
	bool tmpflag = false;
	for (set<string>::iterator it = VT.begin(); it != VT.end(); it++) {
		if ((*it) == "~") {
			tmpflag = true;
		}
	}
	if (!tmpflag) {			//��һ��������Ϊ������ÿ�����
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

//���º���Ϊ���ʱdebug�ã���ʵ�ʳ����в���Ҫ����
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

	readin();		//��������
	delLeftRec();	//������ݹ�
	getFIRST();		//��FIRST����
	getFOLLOW();	//��FOLLOW����
	getTable();		//��Ԥ�������

	in.close();
	out.close();

    return 0;
}