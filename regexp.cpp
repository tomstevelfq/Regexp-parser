#include<iostream>
#include<vector>
#include<string>
#include"utils.h"
#include"NFA.h"
#include"DFA.h"
using namespace std;

//(a|Ab)*(a|b)
//(a|b)*a(a|b)(aa|b)*(a|bb)*
class Regexp{
    public:
        Regexp(){
            n=shared_ptr<NFA>(new NFA());
            d=shared_ptr<DFA>(new DFA());
        }
        shared_ptr<NFA> n;
        shared_ptr<DFA> d;
        void setRegStr(const string& str){
            n->sufstr=suffix(preProcess(str));
            n->build();
            n->buildTable();
            d->build(n);
        }
        bool match(const string& str){
            return d->match(str);
        }
};

int main(){
    // Regexp reg;
    // reg.setRegStr("(a|Ab)*(a|b)");
    // cout<<reg.match("aAbAbab")<<endl;
    cout<<insertContact("(a|Ab)*(a|b)")<<endl;
    cout<<preProcess("(a|Ab)*(a|b)")<<endl;
    return 0;
}