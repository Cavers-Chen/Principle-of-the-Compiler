/*
1. 用C++ 实现；
2. 编程构造如下LR(0)文法的Aciton表和GOTO表，文法G[S]：
       (0) S->E (1)E->aA (2) E->bB (3) A->cA (4) A->d (5) B->cB (6)B->d
3. Aciton表和GOTO表输出到output.txt文件。
4. 提供名为LR0.exe的执行文件实现上述功能，还需提供源文件以及名为设计说明.doc的说明文件。
*/

#pragma warning(disable:4996)

#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

// 文法语句单元
struct statement{  
    // 文法，从->后开始
    char* str;         
    // 非终结符
    char Generator;      
    // 文法序号
    int index;           
    // 当前点的位置，从0开始
    int pos;                                
};
// 状态单元
struct state {
    // 状态序号
    int index;               
    // 是否规约
    bool isReduce;       
    // 语句集合
    vector<statement> statements; 
    // SELECT的后续状态集合
    vector<state> SELECT;        
    // GOTO的后续状态集合
    vector<state> GOTO;           
    // 转换到该状态的输入
    char thisinput;
};

class Solver_LR0 {
public:            
    // 默认第一个式子为S->xxx
    Solver_LR0(vector<char*> language) {
        int i = 0;
        for (const auto l : language) {
            statement s;
            s.str = new char[strlen(l) + 1 - 3];
            strcpy_s(s.str, strlen(l) + 1 - 3, l + 3);
            s.index = i++;
            s.Generator = l[0];
            s.pos = 0;
            Language.statements.push_back(s);
            if (!contain(Generators, s.Generator)) {
                Generators.push_back(s.Generator);
            }
        }
        for (const auto s : Language.statements) {
            for (int i = 0; i < strlen(s.str); i++) {
                if (!contain(Generators, s.str[i]) && !contain(interminate, s.str[i])) {
                    interminate.push_back(s.str[i]);
                }
            }
        }
        Language.index = -1;
        Language.isReduce = false;
    }
    // 判断状态是否一样
    bool equal(statement s1,statement s2) {
        if (s1.index != s2.index || s1.pos != s2.pos)
            return false;
        return true;
    }
    bool equal(state s1, state s2) {        
        if (s1.statements.size() != s2.statements.size())
            return false;
        for (const auto S1 : s1.statements) {
            bool exist = false;
            for (const auto S2 : s2.statements) {
                if (S1.Generator == S2.Generator &&
                    S1.index == S2.index &&
                    S1.pos == S2.pos &&
                    strcmp(S1.str, S2.str) == 0){
                    exist = true;
                    break;
                }
            }
            if (exist == false)
                return false;
        }
        return true;
    }
    bool equal(char c1, char c2) {
        return c1 == c2;
    }
    // 判断vector集合是否存在指定元素
    template<typename T>
    bool contain(vector<T> vec,T element) {
        for (const auto& e : vec) {
            if (equal(e,element))
                return true;
        }
        return false;
    }
    // 构建自动机
    void build() {
        num = 0;
        // 初始化state0
        state state0;
        statement S = Language.statements[0];
        state0.statements.push_back(S);
        bool ischanged = true;
        int index = 0;
        while (ischanged) {
            ischanged = false;
            for (const auto l : state0.statements) {
                if (contain(Generators, l.str[l.pos])) {
                    for (const auto& s : Language.statements) {
                        if (s.Generator == l.str[l.pos] && !contain(state0.statements, s)) {
                            state0.statements.push_back(s);
                            ischanged = true;
                        }
                    }
                }
            }
        }
        // 构建自动机
        queue<state> q;
        state0.index = num++;
        q.push(state0);
        while (!q.empty()) {
            state front = q.front();
            q.pop();
            vector<char> inputs;
            for (const auto s : front.statements) {
                if (!contain(inputs, s.str[s.pos]))
                    inputs.push_back(s.str[s.pos]);
            }
            for (const auto input : inputs) {
                state newstate = getState(front, input);
                if (!contain(all_states, newstate) && !equal(front, newstate)) {
                    //GOTO SELECT HAVEN'T DEFINE!
                    newstate.index = num++;
                    newstate.thisinput = input;
                    if (contain(Generators, input)) {
                        front.GOTO.push_back(newstate);
                    }
                    else {
                        front.SELECT.push_back(newstate);
                    }
                    if (!newstate.isReduce){
                        q.push(newstate);
                    }
                    else
                    {
                        all_states.push_back(newstate);
                    }
                }
                else {
                    // 出现自闭包
                    if (equal(front, newstate))
                    {
                        if (contain(Generators, input)) {
                            if (!contain(front.GOTO, front))
                                front.GOTO.push_back(front);
                        }
                        else {
                            if (!contain(front.SELECT, front))
                                front.SELECT.push_back(front);
                        }
                    }
                    // 出现重复状态
                    else {
                        for (const auto& s : all_states) {
                            if (equal(s, newstate)) {
                                if (contain(Generators, input)) {
                                    front.GOTO.push_back(s);
                                    break;
                                }
                                else {
                                    front.SELECT.push_back(s);
                                    break;
                                }
                        }
                    }
                    }
                }
            }
            all_states.push_back(front);
        }
    }

    state getState(state S,char input) {
        state newstate;
        newstate.isReduce = false;
        for (auto s : S.statements) {
            if (s.str[s.pos] == input) {
                newstate.statements.push_back(s);
            }
        }
        for (int i = 0; i < newstate.statements.size(); i++) {
            newstate.statements[i].pos++;
            if (contain(Generators, newstate.statements[i].str[newstate.statements[i].pos])) {
                for (auto l : Language.statements) {
                    if (l.Generator == newstate.statements[i].str[newstate.statements[i].pos] && !contain(newstate.statements, l)) {
                        l.pos--;
                        newstate.statements.push_back(l);
                    }
                }
            }
        }
        if (newstate.statements.size() == 1 && newstate.statements[0].pos == strlen(newstate.statements[0].str))
            newstate.isReduce = true;
        //newstate.index = all_states.size();
        return newstate;
    }

    void addToFollow(char Gen,char ter) {
        for (auto& g : follow) {
            if (g[0] == Gen && !contain(g, ter)) {
                g.push_back(ter);
            }
        }
    }

    void addToFollow(char Gen1, char Gen2,bool) {
        vector<char> v1, v2;
        for (const auto& f : follow) {
            if (f[0] == Gen1)
                v1 = f;
            if (f[0] == Gen2)
                v2 = f;
        }
        for (int i = 1; i < v2.size(); i++) {
            addToFollow(v1[0], v2[i]);
        }
    }

    void buildFollow() {
        for (auto& g : Generators) {
            vector<char> dumb;
            dumb.push_back(g);
            follow.push_back(dumb);
        }
        // follow
        addToFollow('S', '#');
        for (auto l:Language.statements) {
            for (int i = 0; i < strlen(l.str) - 1; i++) {
                if (contain(Generators, l.str[i]) && contain(interminate, l.str[i + 1])) {
                    addToFollow(l.str[i], l.str[i + 1]);
                }
                if (contain(Generators, l.str[i]) && contain(Generators, l.str[i + 1])) {
                    addToFollow(l.str[i], l.str[i + 1], true);
                }
            }
            if (contain(Generators, l.str[strlen(l.str) - 1]))
            {
                if ((l.str[strlen(l.str) - 1]) != l.Generator)
                    addToFollow((l.str[strlen(l.str) - 1]), l.Generator, true);
            }
        }
    }

    vector<char> getFollow(char Gen) {
        for (auto f : follow) {
            if (f[0] == Gen)
                return f;
        }
    }

    void printTable() {
        buildFollow();
        interminate.push_back('#');

        cout << "index\t";
        for (const auto& input : interminate)
            cout << input << "\t";
        for (const auto& input : Generators)
            cout << input << "\t";
        cout << endl;

        for (int i = 0; i < all_states.size(); i++) {
            cout << all_states[i].index << "\t";
            for (const auto& input : interminate) {
                if (all_states[i].SELECT.size() != 0) {
                    bool flag = true;
                    for (const auto& s : all_states[i].SELECT) {
                        if (s.thisinput == input) {
                            cout << "S" << s.index << "\t";
                            flag = false;
                        }
                    }
                    if (flag)
                        cout << "--\t";
                }
                //else if (all_states[i].isReduce&& contain(getFollow(all_states[i].statements[0].Generator),input)) {
                else if (all_states[i].isReduce) {
                    if (all_states[i].statements[0].index != 0)
                        cout << "R" << all_states[i].statements[0].index << "\t";
                    else if(contain(getFollow(all_states[i].statements[0].Generator), input))
                        cout << "ACC" << "\t";
                    else
                        cout << "--\t";
                }
                else {
                    cout << "--\t";
                }
            }

            for (const auto& input : Generators) {
                if (all_states[i].GOTO.size() != 0){
                    bool flag = true;
                    for (const auto& s : all_states[i].GOTO) {
                        if (s.thisinput == input) {
                            cout << " "<< s.index << "\t";
                            flag = false;
                        }
                    }
                    if (flag)
                        cout << "--\t";
                }
                else {
                    cout << "--\t";
                }
            }

            cout << endl;
        }

        outputfile();
        cout << "文件导出到F盘根目录下..." << endl;
    }

    void outputfile() {
        ofstream out;
        out.open("F:/output.txt");
        out << "index\t";
        for (const auto& input : interminate)
            out << input << "\t";
        for (const auto& input : Generators)
            out << input << "\t";
        out << endl;

        for (int i = 0; i < all_states.size(); i++) {
            out << all_states[i].index << "\t";
            for (const auto& input : interminate) {
                if (all_states[i].SELECT.size() != 0) {
                    bool flag = true;
                    for (const auto& s : all_states[i].SELECT) {
                        if (s.thisinput == input) {
                            out << "S" << s.index << "\t";
                            flag = false;
                        }
                    }
                    if (flag)
                        out << "--\t";
                }
                else if (all_states[i].isReduce) {
                    if (all_states[i].statements[0].index != 0)
                        out << "R" << all_states[i].statements[0].index << "\t";
                    else if (contain(getFollow(all_states[i].statements[0].Generator), input))
                        out << "ACC" << "\t";
                    else
                        out << "--\t";
                }
                else {
                    out << "--\t";
                }
            }

            for (const auto& input : Generators) {
                if (all_states[i].GOTO.size() != 0) {
                    bool flag = true;
                    for (const auto& s : all_states[i].GOTO) {
                        if (s.thisinput == input) {
                            out << " " << s.index << "\t";
                            flag = false;
                        }
                    }
                    if (flag)
                        out << "--\t";
                }
                else {
                    out << "--\t";
                }
            }

            out << endl;
        }
    }

private:
    // 记录状态数
    int num;
    // 记录非终结符
    vector<char> Generators;
    // 初始文法
    state Language;
    // 记录所有的状态
    vector<state> all_states;
    // 记录非终结符
    vector<char> interminate;
    // follow集
    vector<vector<char>> follow;
    // first集
    vector<vector<char>> first;
};

int main() {
    //(0) S->E (1)E->aA (2) E->bB (3) A->cA (4) A->d (5) B->cB (6)B->d
    //char l0[] = "S->E";
    //char l1[] = "E->aA";
    //char l2[] = "E->bB";
    //char l3[] = "A->cA";
    //char l4[] = "A->d";
    //char l5[] = "B->cB";
    //char l6[] = "B->d";

    //language.push_back(l0);
    //language.push_back(l1);
    //language.push_back(l2);
    //language.push_back(l3);
    //language.push_back(l4);
    //language.push_back(l5);
    //language.push_back(l6);

    vector<char*> language;
    cout << "##请将data文件发在F盘根目录下##" << endl;
    cout << "正在读取文件..." << endl;
    fstream f("F:/data.txt");
    string line;
    int setoff = 0;
    while (getline(f, line)) {
        char buffer[999];
        strcpy(buffer + setoff, line.c_str());
        buffer[setoff + strlen(line.c_str())] = '\0';
        language.push_back(buffer + setoff);
        setoff += strlen(line.c_str()) + 1;
    }
    f.close();
    cout << "读取完毕" << endl;

    cout << "正在解析文法..." << endl;
    Solver_LR0 solver(language);
    solver.build();
    cout << "解析完毕" << endl;
    solver.printTable();
    return 0;
}