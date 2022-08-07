#ifndef UTILS_H
#define UTILS_H
#include<iostream>
#include<string>
#include<stack>
#include<algorithm>
using namespace std;
string insertContact(string re) {
    string re_;
    for(int i = 0; i < re.size() - 1; i++) {
        re_ += re[i];
        if(re[i] != '(' && re[i + 1] != ')' && re[i] != '|' && re[i + 1] != '|'
            && re[i + 1] != '*') re_ += '.';
    }
    re_ += re.back();
    return re_;
}
string preProcess(string str){
    string ret;
    int i=0;
    while(i<str.size()){
        if(str[i]=='*'||str[i]=='|'||str[i]==')'){
            ret+=str[i];
            i++; 
        }else if(str[i]=='('){
            if(i!=0&&str[i-1]!='('){
                ret+='.';
            }
            ret+=str[i];
            i++;
        }else{
            if(i!=0&&str[i-1]!='('&&str[i-1]!='|'){
                ret+='.';
            }
            ret+=str[i];
            i++;
        }
    }
    return ret;
}
inline bool operat(char c){
    if(c=='*'||c=='.'||c=='|'){
        return true;
    }
    return false;
}
inline bool priority(char c,char d){
    switch(c){
        case '*':
            if(d=='|'||d=='.'){
                return true;
            }
            break;
        case '.':
            if(d=='|'){
                return true;
            }
            break;
        default:
            return false;
    }
    return false;
}
string suffix(string str){
    string s1;
    stack<char> s2;
    int i=0;
    while(i<str.size()){
        if(operat(str[i])){
            if(!s2.empty()&&operat(s2.top())){
                if(priority(str[i],s2.top())){
                    s2.push(str[i]);
                    i++;
                }else{
                   s1+=s2.top();
                   s2.pop(); 
                }
            }else{
                s2.push(str[i]);
                i++;
            }
        }else if(str[i]==')'){
            while(true){
                if(s2.empty()){
                    cout<<"格式错误"<<endl;
                    exit(0);
                }
                if(s2.top()=='('){
                    s2.pop();
                    i++;
                    break;
                }
                s1+=s2.top();
                s2.pop();
            }

        }else if(str[i]=='('){
            s2.push(str[i]);
            i++;
        }else{
            s1+=str[i];
            i++;
        }
    }
    while(!s2.empty()){
        s1+=s2.top();
        s2.pop();
    }
    return s1;
}
#endif