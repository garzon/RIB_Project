#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

// #define DEBUG_FLAG

#define LONGLONG_BITS (64)
#define BASE_BINARY (2)

using namespace std;

class IP{
    // store IP addr/pattern
public:

    unsigned long long h,l;
    int length;

    static const unsigned long long one;
    static unsigned long long *patterns;

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
            nl=(l>>bit)|((h&(patterns[bit]-one))<<(LONGLONG_BITS-bit));
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
        return (((*p) & patterns[pos]) >> pos);
    }

    inline IP operator ^ (const IP &o) const {
        // assert o.length == length
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
        int patternId=((i>=LONGLONG_BITS)?(i-LONGLONG_BITS):i);
        for(;;--i,--patternId){
            if(patterns[patternId] & src) break;
        }
        ++i;
        return (*shorter) >> i;
    }

#ifdef DEBUG_FLAG
    inline void print() const {
        // for debug
        for(int i=0;i<length;i++)
            printf("%d", (*this)[i]);
        printf("\n");
    }
#endif

};

unsigned long long* IP::patterns = new unsigned long long[LONGLONG_BITS];
const unsigned long long IP::one=static_cast<unsigned long long>(1);

class Node{
    // a Node of binary tree
public:
    IP addr;
    Node *lchild, *rchild, *parent;
    char port1, port2;
    int portLen;

    inline void setPort(const char* _port) {
        if(_port == NULL) { port1 = '\0'; return; }
        port1 = _port[0];
        port2 = _port[1];
        portLen = port2 == '\0' ? 1 : 2;
    }

    inline void getPort(char * &pointer) {
        if(portLen == 0) {
            *(pointer++) = '\n';
            return;
        }
        if(portLen == 2) {
            *(pointer++) = port1;
            *(pointer++) = port2;
            *(pointer++) = '\n';
            return;
        }
        *(pointer++) = port1;
        *(pointer++) = '\n';
    }

    Node(const IP &s, Node *_parent=NULL, const char* _port=NULL):
        addr(s), lchild(NULL), rchild(NULL), parent(_parent), portLen(0)
    {
        setPort(_port);
    }

    ~Node(){
        if(lchild!=NULL) delete lchild;
        if(rchild!=NULL) delete rchild;
    }
};

class RIB{
    Node root;

    Node * _find(Node *curr, const IP &addr, Node **realParent = NULL){
        if(curr==NULL) return NULL;
        if(!(curr->addr < addr)) return NULL;
        if(realParent != NULL)
            if(curr->portLen != 0)
                *realParent = curr;
        int nextBit=addr[curr->addr.length];
        if(nextBit==-1) return curr;  // curr->addr == addr
        Node *nextNode = ((nextBit==0)?(curr->lchild):(curr->rchild));
        Node *res = _find(nextNode, addr, realParent);
        if(res==NULL) return curr; else return res;
    }

    void _delete(Node *p){
        if(p == &root) return;
        Node *parent = p->parent;
        Node **self = (parent->lchild == p) ? (&(parent->lchild)) : (&(parent->rchild));
        if((p->lchild == NULL)&&(p->rchild == NULL)) {
            delete p;
            *self = NULL;
            if(parent->portLen == 0) _delete(parent);
            return;
        }
        if((p->lchild != NULL)&&(p->rchild != NULL)) {
            if(p->portLen == 0) return;
            p->port1 = '\0';
            p->portLen = 0;
            return;
        }
        Node *child = (p->lchild == NULL) ? p->rchild : p->lchild;
        *self = child;
        child->parent = parent;

        p->lchild = NULL; p->rchild = NULL;
        delete p;
    }

    Node * _insert(const IP &addr, const char* port){
        Node *res=_find(&root, addr);
        if(res->addr == addr) {
            res->setPort(port);
            return res;
        }
        int nextBit=addr[res->addr.length];
        Node **nextNode = ((nextBit==0)?(&(res->lchild)):(&(res->rchild)));
        if((*nextNode) == NULL){
            //cout<<"INSERT To: "<<res->port<<" Child:"<<port<<endl;
            (*nextNode) = new Node(addr, res, port);
            return *nextNode;
        }
        if(addr < (*nextNode)->addr){
            Node *newNode = new Node(addr, res, port);
            int nextBit=(*nextNode)->addr[addr.length];
            Node **newNextNode = ((nextBit==0)?(&(newNode->lchild)):(&(newNode->rchild)));
            (*nextNode)->parent = newNode;
            *newNextNode = *nextNode;
            (*nextNode) = newNode;
            //cout<<"INSERT To: "<<res->port<<" Child:"<<port<<" NewChild:"<<(*newNextNode)->port<<endl;
            return newNode;
        }

        IP LCA=addr.getLCA((*nextNode)->addr);
        Node *nLCA=new Node(LCA, res);
        Node *newNode=new Node(addr, nLCA, port);
        if(addr[LCA.length]==0){
            nLCA->lchild=newNode;
            nLCA->rchild=*nextNode;
            nLCA->rchild->parent=nLCA;
        }else{
            nLCA->rchild=newNode;
            nLCA->lchild=*nextNode;
            nLCA->lchild->parent=nLCA;
        }
        *nextNode = nLCA;
        return newNode;
        //cout<<"INSERT To: "<<res->port<<" LCA:"<<nLCA->port<<
        //      " LChild:"<<nLCA->lchild->port<<
        //      " RChild:"<<nLCA->rchild->port<<endl;
    }

public:

    RIB():
        root(IP(""))
    {}

    inline Node* find(const IP &addr){
        Node *realParent = NULL;
        Node *p = _find(&root, addr, &realParent);
        if(p->portLen != 0) return p;
        return realParent;
    }

    void deleteItem(const IP &addr){
        Node *res=_find(&root, addr);
        if (!(res->addr == addr)) return;
        _delete(res);
    }

    void insert(const IP &addr, const char* port){
        _insert(addr, port);
    }
};

int main()
{
    // init -------------------------

    char *buff=new char[128];

    IP::patterns[LONGLONG_BITS-1] = IP::one << (LONGLONG_BITS - 1);
    for(int i=LONGLONG_BITS-2;i>=0;--i) {
        IP::patterns[i] = IP::patterns[i+1] >> 1;
    }

    RIB router;

#ifndef DEBUG_FLAG

    // run phase 1 (offline) --------------

    FILE *f;
    int n,m,l;
    char *port=new char[2];

    f=fopen("nix/RIB2.txt","r");
    fscanf(f,"%d\n",&n);
    for(int i=0;i<n;i++){
        fscanf(f,"%[^/]/%d %s\n", buff, &l, port);
        router.insert(IP(buff, l), port);
    }
    fclose(f);

    // run phase 2 (online) ----------------

    char *outputBuffer = new char [4000000], *outputHead = outputBuffer;
    enum { OP_FIND=1, OP_ADD=2, OP_DEL=3 };

    f=fopen("nix/oper4.txt","r");
    fscanf(f,"%d", &m);
    for(int i=0;i<m;i++) {
        fscanf(f,"%d %s", &n, buff);
        switch(n){
            case OP_FIND:
                router.find(IP(buff))->getPort(outputHead);
                break;
            case OP_ADD:{
                fscanf(f,"%d %s", &l, port);
                router.insert(IP(buff,l), port);
                break;
            }
            case OP_DEL:{
                fscanf(f,"%d %*s", &l);
                router.deleteItem(IP(buff,l));
            }
        }
    }
    fclose(f);

    // output ------------------------------------
    f=fopen("output.txt","w");
    fputs(outputBuffer, f);
    fclose(f);

#else

    // debug -------------------------------------

    scanf("%s[^\n]\n",buff);
    scanf("%s[^\n]\n",buff2);
    IP tmp(buff), tmp2(buff2);
    //for(int i=0;i<LONGLONG_BITS*2;i+=4){
        tmp.getLCA(tmp2).print();
    //}

    //cout<<Node(IP(""),"b").port<<endl;

#endif

    // destroy -------------------------------------

    delete [] buff;
    delete [] port;
    delete [] outputBuffer;
    delete [] IP::patterns;

    return 0;
}

