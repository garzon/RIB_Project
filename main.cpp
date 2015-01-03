#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

// #define DEBUG_FLAG

#define LONGLONG_BITS 64
#define BASE_BINARY 2

using namespace std;

class IP{
    unsigned long long h,l;
    int length;
    static const unsigned long long one;
public:

    IP(char *s, int n=0){
        if(n==0)
            n=strlen(s);
        length=n;
        if(n<=LONGLONG_BITS){
            h=0;
            l=strtoull(s,NULL,BASE_BINARY);
        }else{
            char *p=s+n-LONGLONG_BITS;
            l=strtoull(p,NULL,BASE_BINARY);
            *p='\0';
            h=strtoull(s,NULL,BASE_BINARY);
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
        if(bit>=LONGLONG_BITS){
            nh=0;
            nl=h>>(bit-LONGLONG_BITS);
        }else{
            nl=(l>>bit)+((h&((one<<bit)-one))<<(LONGLONG_BITS-bit));
            nh=h>>bit;
        }
        return IP(nh,nl);
    }

    inline bool operator <(const IP &o) const {
        if(length==0) return true;
        return ((o>>(o.length-length))==(*this));
    }

    inline int operator [](int pos) const {
        const unsigned long long *p;
        if(pos>=LONGLONG_BITS){
            p=&h;
            pos=pos-LONGLONG_BITS;
        }else{
            p=&l;
        }
        return (((*p) & (one<<pos)) >> pos);
    }

    inline void print() const {
        printf("%llX %llX\n", h, l);
    }

};

const unsigned long long IP::one=static_cast<unsigned long long>(1);

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
    IP tmp(buff);
    for(int i=0;i<LONGLONG_BITS*2;i+=4){
        (tmp>>i).print();
    }

#endif

    delete [] buff;
    return 0;
}

