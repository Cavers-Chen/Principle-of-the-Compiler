#pragma once
#include <stack>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

bool isNum_Alp(char ch) {
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) return true;
	if (ch >= '0'&&ch < '9') return true;
	return false;
}

class state {
private:
	vector<state*> next_arr;
	vector<char> next_char_arr;
	size_t name;
	//bool DummyFlag;   if char is 0 then it's a dummy state
public:
	//state(bool isdummy = false) { DummyFlag = isdummy; };
	//bool isDummy() { return DummyFlag; }
	//void setDummy() { this->DummyFlag = true; }
	state(size_t Name) { name = Name; };
	size_t getname() { return name; }
	void add_next(char input, state* next_state) {
		next_arr.push_back(next_state);
		next_char_arr.push_back(input);
	}
	void show() {
		for (int i = 0; i < next_arr.size(); ++i) {
			if (next_char_arr[i] != 0)
				cout << "---" << next_char_arr[i] << "---> state "
					<< next_arr[i]->getname() << endl;
			else
				cout << "---" << "¦Å" << "---> state "
				<< next_arr[i]->getname() << endl;
		}
	}
	vector<char>& getnext_char_arr() {
		return this->next_char_arr;
	}
	vector<state*>& getnext_arr() {
		return this->next_arr;
	}
	bool operator == (state& o) {
		if (o.next_arr.size() == this->next_arr.size() 
			&& o.next_char_arr.size() == this->next_char_arr.size()) {
			for (int i=0; i < o.next_arr.size(); i++) {
				if (o.next_arr[i] != this->next_arr[i]) return false;
			}
			for (int i=0; i < o.next_char_arr.size(); i++) {
				if (o.next_char_arr[i] != this->next_char_arr[i]) return false;
			}
			return true;
		}
		else return false;
	}
};

class states_union {
public:
	states_union(state* h,state*t) {
		this->head = h;
		this->tail = t;
	}
	states_union* CAT(states_union* Union2) {//AB
		this->tail->add_next(0, Union2->head);
		this->tail = Union2->tail;
		return this;
	}
	states_union* OR(states_union* Union2,state*& dummyhead, state*& dummytail, size_t num) {//dummy->A + dummy->B
		//state *dummyhead, *dummytail;
		dummyhead = new state(num);
		dummytail = new state(num+1);
		//attatch dummy to AB's head
		dummyhead->add_next(0, this->head);
		dummyhead->add_next(0, Union2->head);
		this->head = dummyhead;
		//attatch AB's tail to dummy
		this->tail->add_next(0, dummytail);
		Union2->tail->add_next(0, dummytail);
		this->tail = dummytail;
		return this;
	}	
	states_union* POWER_STAR() {
		this->tail->add_next(0, this->head);
		this->head->add_next(0, this->tail);
		return this;
	}
	void POWER_PLUS() {//not easy to do in this case, so I'll solve it soon 
	}
	state* head;
	state* tail;
};

class NFA_TRAN {
private:
	vector<state*> states;
	vector<states_union*> groups;//record address
	stack<states_union*> now;
	state* start, *end;
public:
	NFA_TRAN() { 
		start = end = nullptr;
		start = new state(states.size());
		end = start;
		states.push_back(start);
		states_union* group = new states_union(start, end);
		groups.push_back(group);
		now.push(group);
	}
	//~NFA_TRAN() {
	//	for (size_t i = 0; i < states.size(); i++) {
	//		if (states[i]) {
	//			delete states[i];
	//			states[i] = nullptr;
	//		}
	//	}
	//	for (size_t i = 0; i < groups.size(); i++) {
	//		if (groups[i]) {
	//			delete groups[i];
	//			groups[i] = nullptr;
	//		}
	//	}
	//}
	states_union* LoadExpress(const char* str, size_t& pos, const size_t len);
	states_union* getunit(char input);
	//states_union* Cat(states_union* Union2);
	//states_union* Or(states_union* Union2);
	void MANAGE_STAR(states_union* group);
	void PrintStates();
	state*& operator[](int i) {
		return states[i];
	}
};

states_union* NFA_TRAN::LoadExpress(const char* str, size_t& pos, const size_t len) {
	while (pos < len) {
		if (isNum_Alp(str[pos])) {
			states_union* group = getunit(str[pos]);
			now.top()->CAT(group);
			//now.top()->tail = group->tail;
			if (str[pos + 1] == '*') {
				MANAGE_STAR(group);
				++pos;
			}
		}
		else if (str[pos]=='|'||str[pos]=='+') {
			if (isNum_Alp(str[pos + 1 ])) {
				++pos;
				states_union* group = getunit(str[pos]);
				//now.top()->tail = group->tail;
				if (str[pos] == '*') MANAGE_STAR(group);
				state* newhead = nullptr, *newtail = nullptr;
				now.top()->OR(group, newhead, newtail, states.size());
				states.push_back(newhead);
				states.push_back(newtail);
			}
			else {//is '('
				++pos;
				states_union* newgroup = getunit(0);
				states_union* lastgroup = now.top();
				now.push(newgroup);
				state* newhead = nullptr, *newtail = nullptr;
				states.push_back(newhead);
				states.push_back(newtail);
				lastgroup->OR(LoadExpress(str, pos, len), newhead, newtail, states.size());
				now.pop();
			}
		}
		else if (str[pos]=='(') {
			states_union* newgroup = getunit(0);
			states_union* lastgroup = now.top();
			now.push(newgroup);
			lastgroup->CAT(LoadExpress(str,++pos,len));
			now.pop();
		}
		else if (str[pos]==')') {
			if (str[pos + 1] == '*') {
				MANAGE_STAR(now.top());
				++pos;
			}
			return now.top();
		}
		++pos;
	}
}

states_union* NFA_TRAN::getunit(char input) {
	state* dummyh, *dummyt, *body;
	if (input != 0)
	{
		dummyh = new state(states.size());
		body = new state(states.size()+1);
		dummyt = new state(states.size()+2);
		dummyh->add_next(0, body);
		body->add_next(input, dummyt);
		states.push_back(dummyh);
		states.push_back(body);
		states.push_back(dummyt);
		states_union* group = new states_union(dummyh, dummyt);
		groups.push_back(group);
		return group;
	}
	else {
		dummyh = new state(states.size());
		states.push_back(dummyh);
		states_union* group = new states_union(dummyh, dummyh);
		groups.push_back(group);
		return group;
	}
}

void NFA_TRAN::MANAGE_STAR(states_union* group) {
	group->POWER_STAR();
}

void NFA_TRAN::PrintStates() {
	for (int i = 0; i < states.size(); ++i) {
		if (now.top()->tail->getname() == i) cout << "end state:" << endl;
		cout << "state " << i << " :" << endl;
		states[i]->show();
		cout << endl;
	}
}

class DFA_TRAN {
private:
	vector<vector<state>> states_groups;
	vector<char> characters;
	vector<vector<state>> index; //3 * number of lines
public:
	DFA_TRAN() {}
	void loadcharacters(char* str) {
		for (int i = 0; i < strnlen_s(str, 999); ++i) {
			if ((isalpha(str[i]) || isalnum(str[i]))&&!contain(str[i])) {
				characters.push_back(str[i]);
			}
		}
		characters.push_back(0);
	}
	bool contain(char ch) {
		for (int i=0; i < characters.size(); i++) {
			if (characters[i] == ch)return true;
		}
		return false;
	}
	bool contain(vector<state> v,state key) {
		for (int i = 0; i < v.size(); i++) {
			if (key == v[i])return true;
		}
		return false;
	}
	bool contain(vector<vector<state>> v, vector<state> key) {
		for (int i = 0; i < v.size(); ++i) {
			if (samegroup(v[i], key)) return true;
		}
		return false;
	}
	bool samegroup(vector<state>v1, vector<state>v2) {
		if (v1.size() == v2.size()) {
			for (int i = 0; i < v1.size(); i++) {
				if (!contain(v2, v1[i])) return false;
			}
			return true;
		}
		return false;
	}
	void findthegroup() {
		for (int m = 0; m < states_groups.size(); ++m) {
			for (int j = 0; j < characters.size() - 1; ++j) {
				vector<state> states = states_groups[m];
				vector<state> newstates;
				char input = characters[j];
				for (int i = 0; i < states.size(); ++i) {
					for (int x = 0; x < states[i].getnext_char_arr().size();x++) {
						state s = *(states[i].getnext_arr())[x];
						char ch = (states[i].getnext_char_arr())[x];
						if ((ch == input || ch == 0) && !contain(newstates, s)) {
							newstates.push_back(s);
							if (ch == 0) states.push_back(s);
						}
					}
				}
				if (!contain(states_groups, newstates))
					states_groups.push_back(newstates);
				index.push_back(newstates);
			}
		}
	}
	void buildtable(NFA_TRAN nfa) {
		vector<state> firstgroup;
		firstgroup.push_back(*nfa[0]);
		states_groups.push_back(firstgroup);
		findthegroup();
	}
	void print() {
		for (int i = 0; i < characters.size(); ++i)
			cout<<"  " << characters[i] << "  ";
		cout << endl;
		for (int i = 0; i < states_groups.size(); ++i) {
			    printgroupsname(states_groups[i]);
				for (int j = 0 + (characters.size() - 1)*i;
					j < (characters.size() - 1)*(i + 1); j++) {
					printgroupsname(index[j]);
			}
				cout << endl;
		}
	}
	void printgroupsname(vector<state> o) {
		cout << "{";
		for (int i = 0; i < o.size(); ++i) {
			if (i == 0)
				cout << o[i].getname();
			else
				cout <<','<< o[i].getname();
		}
		cout << "}";
	}
};