#ifndef UTILS_H
#define UTILS_H
#include<iostream>
#include<string>
#include<stack>
#include<algorithm>
using namespace std;
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
    stack<char> s1;
    stack<char> s2;
    int i=0;
    while(i<str.size()){
        if(operat(str[i])){
            if(!s2.empty()&&operat(s2.top())){
                if(priority(str[i],s2.top())){
                    s2.push(str[i]);
                    i++;
                }else{
                   s1.push(s2.top());
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
                s1.push(s2.top());
                s2.pop();
            }

        }else if(str[i]=='('){
            s2.push(str[i]);
            i++;
        }else{
            s1.push(str[i]);
            i++;
        }
    }
    while(!s2.empty()){
        s1.push(s2.top());
        s2.pop();
    }
    string ret;
    while(!s1.empty()){
        ret+=s1.top();
        s1.pop();
    }
    reverse(ret.begin(),ret.end());
    return ret;
}
#endif