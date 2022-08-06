#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<memory>
using namespace std;

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
    void accept(Visitor* v,const string& blk);
}*NodeTree,Node;
struct Visitor{
    void visit(Node* p,const string& blk);
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
void Node::accept(Visitor* v,const string& blk){
    v->visit(this,blk);
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
void Visitor::visit(Node* p,const string& blk){
    char ope[3]={'*','+',' '};
    string newblk=blk+"   ";
    cout<<blk<<p->name<<": "<<p->val<<endl;
    for(auto it:p->children){
        it->accept(this,newblk);
    }
}

//[0-9a-z]fsf*.+(dd)*a
int main(){
    Parser par;
    par.regstr="[0-9a-z]fsf*.+(dd)*a";
    Visitor vis;
    auto p=par.execute();
    vis.visit(p.get(),"");
    return 0;
}