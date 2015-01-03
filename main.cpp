#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

// #define DEBUG_FLAG

using namespace std;

class IP{
    unsigned long long h,l;
    int length;
public:

    IP(char *s){
        int &n=length;
        n=strlen(s);
        if(n<65){
            h=0;
            l=strtoull(s,NULL,2);
        }else{
            char *p=s+n-64;
            l=strtoull(p,NULL,2);
            *p='\0';
            h=strtoull(s,NULL,2);
        }
    }

    IP(unsigned long long _h, unsigned long long _l):
        h(_h), l(_l)
    {}

    inline int getLength() const {
        return length;
    }

    inline bool operator == (const IP &o) const {
        return (o.h==h)&&(o.l==l);
    }

    inline IP operator >> (int bit) const {
        unsigned long long nh,nl;
        if(bit>63){
            nh=0;
            nl=h>>(bit-64);
        }else{
            nl=(l>>bit)+((h&((1<<bit)-1))<<(64-bit));
            nh=h>>bit;
        }
        return IP(nh,nl);
    }

    inline bool operator <(const IP &o) const {
        if(length==0) return true;
        return ((o>>(o.length-length))==(*this));
    }

    inline void print() const {
        printf("%llX %llX\n", h, l);
    }

};

class Node{
public:
    IP addr;
    Node *lchild, *rchild;
    Node(const IP &s):
        addr(s), lchild(NULL), rchild(NULL)
    {}
    ~Node(){
        if(lchild!=NULL) delete lchild;
        if(rchild!=NULL) delete rchild;
    }
};

class RIB{
    Node root;
public:
    RIB():
        root(IP(""))
    {}
};

int main()
{
    char *buff=new char[1000];

#ifndef DEBUG_FLAG

    FILE *f;
    f=fopen("RIB.txt","r");
    int n,m;
    fscanf(f,"%d\n",&n);
    for(int i=0;i<n;i++){

    }
    fclose(f);

#else

    scanf("%s[^\n]",buff);
    cout<<(IP("").getLength())<<endl;

#endif

    delete [] buff;
    return 0;
}

