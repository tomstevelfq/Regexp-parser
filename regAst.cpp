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
    Node(NodeType t):type(t){}
    Node(NodeType t,const string& name):type(t),name(name){}
    int sta;
    char val;
    NodeType type;
    string name;
    vector<nptr> children;
    void add(nptr p);
    pair<int,int> accept(Visitor* v);
}*NodeTree,Node;
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
};
struct Parser{
    int pos=0;
    char token;
    string regstr;
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
};
void Node::add(nptr p){
    children.push_back(p);
}
pair<int,int> Node::accept(Visitor* v){
    return v->visit(this);
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
    // char ope[3]={'*','+',' '};
    // string newblk=blk+"   ";
    // cout<<blk<<p->name<<": "<<p->val<<endl;
    // for(auto it:p->children){
    //     it->accept(this,newblk);
    // }

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
        case GROUP:
            par=p->children[0]->accept(this);
            pair<int,int> p1=newNode('(');
            pair<int,int> p2=newNode(')');
            par=connect({p1,par,p2});
            break;
        case ANY:
            par=newNode('.');
            break;
        case CHAR:
            par=newNode(p->val);
            break;
        case SET:
            par=connect(newNode('['),newNode(']'));
            // if(p->sta==0){

            // }else if(p->sta==1){

            // }else{
            //     error();
            // }
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
    // n1->END->add('@',n2->STA);
    // n1->END=n2->END;
    // return n1;
    //table[p1.second].insert(make_pair('@',p2.first));
    addEdge(p1.second,p2.first,nir);
    p1.second=p2.second;
    return p1;
}
pair<int,int> Visitor::parallel(pair<int,int> p1,pair<int,int> p2){
    // auto n=shared_ptr<NFA>(new NFA());
    // n->STA->add('@',n1->STA).add('@',n2->STA);
    // n1->END->add('@',n->END);
    // n2->END->add('@',n->END);
    // return n;
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
    // auto n=shared_ptr<NFA>(new NFA());
    // n->STA->add('@',n1->STA).add('@',n->END);
    // n1->END->add('@',n1->STA).add('@',n->END);
    // return n;
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
    par.regstr="[0-9a-z]fsf*.+(dd)*a";
    Visitor vis;
    auto p=par.execute();
    p->accept(new Visitor());
    return 0;
}