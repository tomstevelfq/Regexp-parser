#ifndef DFA_H
#define DFA_H
#include "NFA.h"
#include "utils.h"
#include<map>
using namespace std;

struct DFA{
    bool isEnd(int sta);
    set<int> End;
    vector<unordered_map<char,int>> table;
    shared_ptr<NFA> n;
    set<char> symbols;
    void build(shared_ptr<NFA>);
    set<int> clousure(set<int>);
    set<int> move(set<int>,char);
    bool match(const string& str);
};
void DFA::build(shared_ptr<NFA> n){
    this->n=n;
    const string& suf=n->sufstr;
    for(int i=0;i<suf.size();i++){
        if(suf[i]!='|'&&suf[i]!='.'&&suf[i]!='*'){
            symbols.insert(suf[i]);
        }
    }
    vector<pair<set<int>,map<char,set<int>>>> stable;
    auto s0=clousure(set<int>{n->STA->num});
    stack<set<int>> dataset;
    set<set<int>> mark;
    dataset.push(s0);
    mark.insert(s0);
    while(!dataset.empty()){
        auto T=dataset.top();
        dataset.pop();
        map<char,set<int>> m;
        for(auto ch:symbols){
            auto U=clousure(move(T,ch));
            if(!U.empty()){
                if(mark.find(U)==mark.end()){
                    dataset.push(U);
                    mark.insert(U);
                }
                m.insert(make_pair(ch,U));
            }
        }
        stable.push_back(make_pair(T,m));
    }
    map<set<int>,int> temtable;
    int pos=0;
    int endnum=n->END->num;
    for(auto& it:stable){
        if(it.first.find(endnum)!=it.first.end()){
            End.insert(pos);
        }
        temtable.insert(make_pair(it.first,pos++));
    }
    for(auto& it:stable){
        unordered_map<char,int> tem;
        auto m=it.second;
        for(auto& mit:m){
            tem.insert(make_pair(mit.first,temtable.find(mit.second)->second));
        }
        table.push_back(tem);
    }
    return;
}
bool DFA::isEnd(int sta){
    if(End.find(sta)!=End.end()){
        return true;
    }
    return false;
}
set<int> DFA::clousure(set<int> s){
    set<int> ret;
    stack<int> st;
    auto& table=n->table;
    for(auto it:s){
        st.push(it);
        ret.insert(it);
    }
    while(!st.empty()){
        int tem=st.top();
        st.pop();
        auto it=table[tem].find('@');
        if(it!=table[tem].end()){
            for(auto sta:it->second){
                if(ret.find(sta)==ret.end()){
                    ret.insert(sta);
                    st.push(sta);
                }
            }
        }
    }
    return ret;
}
set<int> DFA::move(set<int> s,char a){
    set<int> ret;
    auto table=n->table;
    for(auto it:s){
        auto tem=table[it].find(a);
        if(tem!=table[it].end()){
            for(auto sta:tem->second){
                ret.insert(sta);
            }
        }
    }
    return ret;
}
bool DFA::match(const string& str){
    int i=0;
    int state=0;
    while(i<str.size()){
        auto it=table[state].find(str[i]);
        if(it==table[state].end()){
            break;
        }
        state=it->second;
        i++;
    }
    if(i==str.size()&&isEnd(state)){
        return true;
    }
    return false;
}
#endif