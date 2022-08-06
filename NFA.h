#ifndef NFA_H
#define NFA_H
#include<iostream>
#include<vector>
#include<string>
#include<stack>
#include<algorithm>
#include<unordered_map>
#include<memory>
#include<set>
#include"utils.h"
using namespace std;
int snum=0;
typedef struct State{
    State(){num=snum++;}
    int num;
    unordered_map<char,vector<shared_ptr<State>>> m;
    State& add(char c,shared_ptr<State> s);
}State;
typedef struct NFA{
    NFA(){
        STA=shared_ptr<State>(new State());
        END=shared_ptr<State>(new State());
    }
    shared_ptr<State> STA;
    shared_ptr<State> END;
    int stanum=0;
    //转换表
    vector<unordered_map<char,set<int>>> table;
    string str;
    string sufstr;
    bool match(shared_ptr<State>(now),int pos);
    bool tableMatch(int now,int pos);
    void build();
    void buildTable();
    void buildt(vector<bool>& vis,shared_ptr<State> now);
}NFA;
shared_ptr<NFA> genNFA(char c){
    auto N=shared_ptr<NFA>(new NFA());
    N->STA->add(c,N->END);
    return N;
}
shared_ptr<NFA> connect(shared_ptr<NFA> n1,shared_ptr<NFA> n2){
    n1->END->add('@',n2->STA);
    n1->END=n2->END;
    return n1;
}
shared_ptr<NFA> parallel(shared_ptr<NFA> n1,shared_ptr<NFA> n2){
    auto n=shared_ptr<NFA>(new NFA());
    n->STA->add('@',n1->STA).add('@',n2->STA);
    n1->END->add('@',n->END);
    n2->END->add('@',n->END);
    return n;
}
shared_ptr<NFA> repeat(shared_ptr<NFA> n1){
    auto n=shared_ptr<NFA>(new NFA());
    n->STA->add('@',n1->STA).add('@',n->END);
    n1->END->add('@',n1->STA).add('@',n->END);
    return n;
}

State& State::add(char c,shared_ptr<State> s){
    auto v=m.find(c);
    if(v!=m.end()){
        v->second.push_back(s);
    }else{
        m.insert(pair<char,vector<shared_ptr<State>>>(c,{s}));
    }
    return *this;
}
int num=0;
bool NFA::match(shared_ptr<State> now,int pos){
    num++;
    if(now->m.find('@')!=now->m.end()){
        auto v=now->m.find('@')->second;
        for(auto it:v){
            if(match(it,pos)){
                return true;
            }
        }
    }
    if(now==END||pos==str.size()){
        if(now==END&&pos==str.size()){
            return true;
        }
        return false;
    }
    if(now->m.find(str[pos])!=now->m.end()){
        auto v=now->m.find(str[pos])->second;
        for(auto it:v){
            if(match(it,pos+1)){
                return true;
            }
        }
    }
    return false;
}
bool NFA::tableMatch(int now,int pos){
    num++;
    if(table[now].find('@')!=table[now].end()){
        auto& v=table[now].find('@')->second;
        for(auto it:v){
            if(tableMatch(it,pos)){
                return true;
            }
        }
    }
    if(now==END->num||pos==str.size()){
        if(now==END->num&&pos==str.size()){
            return true;
        }
        return false;
    }
    if(table[now].find(str[pos])!=table[now].end()){
        auto v=table[now].find(str[pos])->second;
        for(auto it:v){
            if(tableMatch(it,pos+1)){
                return true;
            }
        }
    }
    return false;
}
void NFA::build(){
    snum=0;
    stack<shared_ptr<NFA>> s;
    int i=0;
    while(i<sufstr.size()){
        char c=sufstr[i];
        if(operat(c)){
            if(c=='*'){
                auto tem=repeat(s.top());
                s.pop();
                s.push(tem);
            }else if(c=='|'){
                auto n2=s.top();
                s.pop();
                auto n1=s.top();
                s.pop();
                s.push(parallel(n1,n2));
            }else if(c=='.'){
                auto n2=s.top();
                s.pop();
                auto n1=s.top();
                s.pop();
                s.push(connect(n1,n2)); 
            }
        }else{
            s.push(genNFA(c));
        }
        i++;
    }
    END=s.top()->END;
    STA=s.top()->STA;
    stanum=snum;
}
void NFA::buildTable(){
    vector<bool> vis(stanum,0);
    table.resize(stanum);
    buildt(vis,STA);
}
void NFA::buildt(vector<bool>& vis,shared_ptr<State> now){
    if(vis[now->num]){
        return;
    }
    vis[now->num]=true;
    int s=now->num;
    set<shared_ptr<State>> tem;
    for(auto& it:(now->m)){
        char c=it.first;
        if(table[s].find(c)==table[s].end()){
            table[s].insert(make_pair(c,set<int>()));
        }
        auto p=table[s].find(c);
        for(auto& sta:it.second){
            tem.insert(sta);
            p->second.insert(sta->num);
        }
    }
    for(auto& sta:tem){
        buildt(vis,sta);
    }
}
#endif