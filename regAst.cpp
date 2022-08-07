#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<memory>
#include<map>
#include<stack>
using namespace std;

const char nir='@';
enum NodeType{RE,SIMPLE,BASICRC,ELEMRC,GROUP,ANY,CHAR,SET,ITEMS,ITEM,RANGE};
set<char> meta{'(','.','*','+',')','^','[',']'};
void error(){
    cout<<"error match"<<endl;
    exit(0);
}
bool isMeta(char c){
    if(meta.find(c)==meta.end()){
        return false;
    }
    return true;
}
struct Node;
struct Visitor;
typedef shared_ptr<Node> nptr;
typedef struct Node{
    Node(NodeType t):type(t),name(""),val(' '),sta(0){}
    Node(NodeType t,const string& name):type(t),name(name),val(' '),sta(0){}
    shared_ptr<Visitor> pv;
    int sta;
    char val;
    NodeType type;
    string name;
    vector<nptr> children;
    void add(nptr p);
    pair<int,int> acc(shared_ptr<Visitor>);
    pair<int,int> accept(Visitor* v);
    void showTable();
}*NodeTree,Node;
void print(nptr p,const string& blk){
    cout<<blk<<p->name<<": "<<p->val<<" sta: "<<p->sta<<endl;
    for(auto it:p->children){
        print(it,blk+"  ");
    }
}

struct Visitor{
    vector<map<char,set<int>>> table;
    int snum=0;
    int newsta();
    pair<int,int> newNode();
    pair<int,int> newNode(char c);
    pair<int,int> visit(Node* p);
    pair<int,int> connect(pair<int,int> p1,pair<int,int> p2);
    pair<int,int> connect(vector<pair<int,int>>);
    pair<int,int> parallel(pair<int,int> p1,pair<int,int> p2);
    pair<int,int> repeat(pair<int,int> p1);
    void addEdge(int s1,int s2,char c);
    void showTable();
};
struct Parser{
    int pos=0;
    char token;
    int END;
    int STA;
    string regstr;
    string matstr;
    vector<map<char,set<int>>> table;
    nptr npr;
    nptr execute();
    char nextch();
    void ungetch();
    void match(char c);
    nptr re();
    nptr simple();
    nptr basicrc();
    nptr elemrc();
    nptr group();
    nptr any();
    nptr ch();
    nptr set();
    nptr items();
    nptr item();
    nptr getTree();
    bool matchstr(const string& str);
    bool match_(int sta,int pos);
};
void Node::add(nptr p){
    children.push_back(p);
}
pair<int,int> Node::accept(Visitor* v){
    return v->visit(this);
}
pair<int,int> Node::acc(shared_ptr<Visitor> v){
    pv=v;
    return accept(pv.get());
}
void Node::showTable(){
    pv->showTable();
}
nptr Parser::getTree(){
    if(npr.get()==NULL){
        return execute();
    }
    return npr;
}
bool Parser::matchstr(const string& str){
    auto p=getTree();
    //print(p,"");
    auto pair_=p->acc(shared_ptr<Visitor>(new Visitor()));
    //设置NFA的开始和终结状态
    END=pair_.second;
    STA=pair_.first;
    table=p->pv->table;
    matstr=str;
    return match_(STA,0);
}
int num=0;
bool Parser::match_(int now,int pos){
    num++;
    if(table[now].find('@')!=table[now].end()){
        auto v=table[now].find('@')->second;
        for(auto it:v){
            if(match_(it,pos)){
                return true;
            }
        }
    }
    if(now==END||pos==matstr.size()){
        if(now==END&&pos==matstr.size()){
            return true;
        }
        return false;
    }
    if(table[now].find(matstr[pos])!=table[now].end()){
        auto v=table[now].find(matstr[pos])->second;
        for(auto it:v){
            if(match_(it,pos+1)){
                return true;
            }
        }
    }
    return false;
}
nptr Parser::execute(){
    token=nextch();
    return re();
}
void Parser::match(char c){
    if(token!=c){
        error();
    }
    token=nextch();
}
char Parser::nextch(){
    return regstr[pos++];
}
void Parser::ungetch(){
    pos--;
}
nptr Parser::re(){
    nptr ret=nptr(new Node(RE,"RE"));
    ret->add(simple());
    while(token=='|'){
        match(token);
        ret->add(simple());
    }
    return ret;
}
nptr Parser::simple(){
    nptr ret=nptr(new Node(SIMPLE,"SIMPLE"));
    ret->add(basicrc());
    while(token!='\0'&&token!='|'&&token!=')'){
        ret->add(basicrc());
    }
    return ret;
}
nptr Parser::basicrc(){
    auto ret=nptr(new Node(BASICRC,"BASICRC"));
    ret->add(elemrc());
    if(token=='*'){
        match('*');
        ret->sta=0;
    }else if(token=='+'){
        match('+');
        ret->sta=1;
    }else{
        ret->sta=2;
    }
    return ret;
}
nptr Parser::elemrc(){
    auto ret=nptr(new Node(ELEMRC,"ELEMRC"));
    if(!isMeta(token)){
        ret->add(ch());
    }else if(token=='.'){
        ret->add(any());
    }else if(token=='('){
        ret->add(group());
    }else if(token=='['){
        ret->add(set());
    }else{
        error();
    }
    return ret;
}
nptr Parser::group(){
    auto ret=nptr(new Node(GROUP,"GROUP"));
    match('(');
    ret->add(re());
    match(')');
    return ret;
}
nptr Parser::any(){
    auto ret=nptr(new Node(ANY,"ANY"));
    match('.');
    ret->val='.';
    return ret;
}
nptr Parser::set(){
    auto ret=nptr(new Node(SET,"SET"));
    match('[');
    if(token=='^'){
        match('^');
        ret->add(items());
        ret->sta=0;
    }else{
        ret->add(items());
        ret->sta=1;
    }
    match(']');
    return ret;
}
nptr Parser::items(){
    auto ret=nptr(new Node(ITEMS,"ITEMS"));
    ret->add(item());
    while(token!=']'){
        ret->add(item());
    }
    return ret;
}
nptr Parser::item(){
    auto ret=nptr(new Node(ITEM,"ITEM"));
    auto tem=ch();
    if(token=='-'){
        match('-');
        ret->add(tem);
        ret->add(ch());
        ret->sta=1;
    }else{
        ret->add(tem);
        ret->sta=0;
    }
    return ret;
}
nptr Parser::ch(){
    auto ret=nptr(new Node(CHAR,"CHAR"));
    if(isMeta(token)){
        error();
    }
    if(token!='/'){
        ret->val=token;
        match(token);
    }else{
        match('/');
        ret->val=token;
        match(token);
    }
    return ret;
}
pair<int,int> Visitor::visit(Node* p){
    //生成两个状态 开始 终结
    stack<pair<int,int>> tem;
    pair<int,int> par;
    switch(p->type){
        case RE:
            par=p->children[0]->accept(this);
            for(int i=1;i<p->children.size();i++){
                par=parallel(par,p->children[i]->accept(this));
            }
            break;
        case SIMPLE:
            par=p->children[0]->accept(this);
            for(int i=1;i<p->children.size();i++){
                par=connect(par,p->children[i]->accept(this));
            }
            break;
        case BASICRC:
            par=p->children[0]->accept(this);
            if(p->sta==0){
                par=repeat(par);
            }else if(p->sta==1){
                par=connect(par,repeat(par));
            }else if(p->sta!=2){
                error();
            }
            break;
        case ELEMRC:
            par=p->children[0]->accept(this);
            break;
        case GROUP:{
            par=p->children[0]->accept(this);
            break;
        }
        case ANY:
            par=newNode('.');
            break;
        case CHAR:
            par=newNode(p->val);
            break;
        case SET:
            par=connect(newNode('['),newNode(']'));
            break;
        case ITEMS:
            par=connect(newNode('['),newNode(']'));
            break;
        case ITEM:
            par=connect(newNode('['),newNode(']'));
            break;
        case RANGE:
            par=connect(newNode('['),newNode(']'));
            break;
        default:
            error();
            break;
    }
    return par;
}
void Visitor::showTable(){
    int p=0;
    string bk="    ";
    for(auto it:table){
        cout<<p++<<": "<<endl;;
        for(auto tem:it){
            cout<<bk<<tem.first<<": ";
            for(auto tem1:tem.second){
                cout<<tem1<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }
}
int Visitor::newsta(){
    table.push_back(map<char,set<int>>());
    return snum++;
}
pair<int,int> Visitor::newNode(){
    return pair<int,int>{newsta(),newsta()};
}
pair<int,int> Visitor::newNode(char c){
    pair<int,int> ret={newsta(),newsta()};
    addEdge(ret.first,ret.second,c);
    return ret;
}
pair<int,int> Visitor::connect(pair<int,int> p1,pair<int,int> p2){
    addEdge(p1.second,p2.first,nir);
    p1.second=p2.second;
    return p1;
}
pair<int,int> Visitor::parallel(pair<int,int> p1,pair<int,int> p2){
    pair<int,int> p=newNode();
    addEdge(p.first,p1.first,nir);
    addEdge(p.first,p2.first,nir);
    addEdge(p1.second,p.second,nir);
    addEdge(p2.second,p.second,nir);
    return p;
}
pair<int,int> Visitor::connect(vector<pair<int,int>> v){
    pair<int,int> par=v[0];
    for(int i=1;i<v.size();i++){
        par=connect(par,v[i]);
    }
    return par;
}
pair<int,int> Visitor::repeat(pair<int,int> p1){
    pair<int,int> p=newNode();
    addEdge(p.first,p1.first,nir);
    addEdge(p.first,p.second,nir);
    addEdge(p1.second,p1.first,nir);
    addEdge(p1.second,p.second,nir);
    return p;
}
void Visitor::addEdge(int s1,int s2,char c){
    auto it=table[s1].find(c);
    if(it==table[s1].end()){
        table[s1].insert(make_pair(c,set<int>{s2}));
    }else{
        it->second.insert(s2);
    }
}

//[0-9a-z]fsf*.+(dd)*a
int main(){
    Parser par;
    par.regstr="(a|Ab)*(a|b)";
    cout<<par.matchstr("aAbAbab")<<endl;
    cout<<num<<endl;
    return 0;
}