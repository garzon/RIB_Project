#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

// #define DEBUG_FLAG

#define LONGLONG_BITS (64)
#define BASE_BINARY (2)

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

    IP(unsigned long long _h, unsigned long long _l, int _length):
        h(_h), l(_l), length(_length)
    {}

    inline int getLength() const {
        return length;
    }

    inline bool operator == (const IP &o) const {
        // same
        return (o.length==length)&&(o.h==h)&&(o.l==l);
    }

    inline IP operator >> (int bit) const {
        // shift right to compare
        unsigned long long nh,nl;
        if(bit>=LONGLONG_BITS){
            nh=0;
            nl=h>>(bit-LONGLONG_BITS);
        }else{
            nl=(l>>bit)|((h&((one<<bit)-one))<<(LONGLONG_BITS-bit));
            nh=h>>bit;
        }
        return IP(nh,nl,length-bit);
    }

    inline bool operator <(const IP &o) const {
        // when true: *this match o
        if(length==0) return true;
        if(o.length<length) return false;
        return ((o>>(o.length-length))==(*this));
    }

    inline bool operator >(const IP &o) const {
        // when true: *this not match o
        return !((*this)<o);
    }

    inline int operator [](int pos) const {
        // get a bit @ pos
        if(pos>=length) return -1;
        pos=length-pos-1;
        const unsigned long long *p;
        if(pos>=LONGLONG_BITS){
            p=&h;
            pos=pos-LONGLONG_BITS;
        }else{
            p=&l;
        }
        return (((*p) & (one<<pos)) >> pos);
    }

    inline IP operator ^ (const IP &o) const {
        if(o.length != length){
            throw exception();
        }
        return IP(o.h^h, o.l^l, length);
    }

    IP getLCA(const IP &o) const {
        int minlen;
        IP longer("");
        const IP *shorter;
        if(o.length>length){
            minlen=length;
            longer=o>>(o.length-minlen);
            shorter=this;
        }else{
            minlen=o.length;
            shorter=&o;
            longer=(*this)>>(length-minlen);
        }
        longer = longer ^ (*shorter);
        unsigned long long &src=(longer.h==0)?longer.l:longer.h;
        int i=(longer.h==0)?min(minlen-1,LONGLONG_BITS-1):(minlen-1);
        unsigned long long pattern=one<<((i>=LONGLONG_BITS)?(i-LONGLONG_BITS):i);
        for(;;--i){
            if(pattern & src) break;
            pattern >>= 1;
        }
        ++i;
        return (*shorter) >> i;
    }

    inline void print() const {
        // for debug
        printf("%llX %llX\n", h, l);
    }

};

const unsigned long long IP::one=static_cast<unsigned long long>(1);

class Node{
    // a Node of binary tree
public:
    IP addr;
    Node *lchild, *rchild, *parent;
    string port;

    Node(const IP &s, Node *_parent=NULL, const char *_port=NULL):
        addr(s), lchild(NULL), rchild(NULL), parent(_parent), port(_port==NULL?"":_port)
    {}

    ~Node(){
        if(lchild!=NULL) delete lchild;
        if(rchild!=NULL) delete rchild;
    }
};

class RIB{
    Node root;

    Node * _find(Node *curr, const IP &addr){
        if(curr==NULL) return NULL;
        if(curr->addr > addr) return NULL;
        int nextBit=addr[curr->addr.getLength()];
        if(nextBit==-1) return curr;  // curr->addr == addr
        Node *nextNode = ((nextBit==0)?(curr->lchild):(curr->rchild));
        Node *res = _find(nextNode, addr);
        if(res==NULL) return curr; else return res;
    }

public:

    RIB():
        root(IP(""))
    {}

    inline const string & find(const IP &addr){
        return _find(&root, addr)->port;
    }

    void deleteItem(){

    }

    void insert(const IP &addr, const char *port){
        Node *res=_find(&root, addr);
        if(res->addr == addr){
            res->port=string(port);
            return;
        }
        int nextBit=addr[res->addr.getLength()];
        Node **nextNode = ((nextBit==0)?(&(res->lchild)):(&(res->rchild)));
        if((*nextNode) == NULL){
            //cout<<"INSERT To: "<<res->port<<" Child:"<<port<<endl;
            (*nextNode) = new Node(addr, res, port);
            return;
        }
        if(addr < (*nextNode)->addr){
            Node *newNode = new Node(addr, res, port);
            int nextBit=(*nextNode)->addr[addr.getLength()];
            Node **newNextNode = ((nextBit==0)?(&(newNode->lchild)):(&(newNode->rchild)));
            (*nextNode)->parent = newNode;
            *newNextNode = *nextNode;
            (*nextNode) = newNode;
            //cout<<"INSERT To: "<<res->port<<" Child:"<<port<<" NewChild:"<<(*newNextNode)->port<<endl;
            return;
        }

        IP LCA=addr.getLCA((*nextNode)->addr);
        Node *nLCA=new Node(LCA, res);
        Node *newNode=new Node(addr, nLCA, port);
        if(addr[LCA.getLength()]==0){
            nLCA->lchild=newNode;
            nLCA->rchild=*nextNode;
            nLCA->rchild->parent=nLCA;
        }else{
            nLCA->rchild=newNode;
            nLCA->lchild=*nextNode;
            nLCA->lchild->parent=nLCA;
        }
        *nextNode = nLCA;
        //cout<<"INSERT To: "<<res->port<<" LCA:"<<nLCA->port<<
        //      " LChild:"<<nLCA->lchild->port<<
        //      " RChild:"<<nLCA->rchild->port<<endl;
    }
};

int main()
{
    char *buff=new char[1000];
    char *buff2=new char[1000];

    RIB router;

#ifndef DEBUG_FLAG

    FILE *f;

    f=fopen("RIB.txt","r");
    int n,m,l; char tmp='t';
    fscanf(f,"%d\n",&n);
    for(int i=0;i<n;i++){
        fscanf(f,"%[^/]/%d %[^\n]\n", buff, &l, buff2);
        router.insert(IP(buff, l), buff2);
    }
    fclose(f);

#else

    /*
    scanf("%s[^\n]",buff);
    IP tmp(buff);
    for(int i=0;i<LONGLONG_BITS*2;i+=4){
        (tmp>>i).print();
    }
    */
    //cout<<Node(IP(""),"b").port<<endl;

#endif

    delete [] buff;
    delete [] buff2;
    return 0;
}

