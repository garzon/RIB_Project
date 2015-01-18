#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

// #define DEBUG_FLAG

#define LONGLONG_BITS (64)
#define BASE_BINARY (2)
#define PORT_LEN (2)

using namespace std;

class Node{
    // a Node of binary tree
public:
    Node *lchild, *rchild, *parent, **parentSelf;
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

    Node(Node *_parent, Node **_self):
        lchild(NULL), rchild(NULL), parent(_parent), parentSelf(_self), portLen(0)
    {}

    ~Node() {
        if(lchild != NULL) delete lchild;
        if(rchild != NULL) delete rchild;
        if(parentSelf != NULL) *parentSelf = NULL;
    }
};

class RIB{
    Node root;

    Node * _find(Node *curr, const char *addr, bool createNew, Node **realParent = NULL) {
        if(realParent != NULL) {
            if(curr->portLen != 0) {
                *realParent = curr;
            }
        }
        if(*addr == '\0') return curr;
        if(*addr == '0') {
            if(curr->rchild == NULL) {
                if(createNew)
                    curr->rchild = new Node(curr, &(curr->rchild));
                else return curr;
            }
            return _find(curr->rchild, ++addr, createNew, realParent);
        } else {
            if(curr->lchild == NULL) {
                if(createNew)
                    curr->lchild = new Node(curr, &(curr->lchild));
                else return curr;
            }
            return _find(curr->lchild, ++addr, createNew, realParent);
        }
    }

    void _delete(Node *p){
        Node *parent = p->parent;
        delete p;
        if(parent != &root) {
            if(parent->lchild == NULL)
                if(parent->rchild == NULL)
                    if(parent->portLen == 0) {
                        _delete(parent);
                    }
        }
    }

public:

    RIB():
        root(NULL, NULL)
    {}

    inline Node* find(const char* addr) {
        Node *realParent;
        Node *res = _find(&root, addr, false, &realParent);
        return res->portLen == 0 ? realParent : res;
    }

    inline void remove(const char *addr) {
        _delete(_find(&root, addr, true));
    }

    inline void insert(const char *addr, const char* port) {
        _find(&root, addr, true)->setPort(port);
    }
};

int main()
{
    // init -------------------------

    RIB router;

#ifndef DEBUG_FLAG

    // run phase 1 (offline) --------------

    FILE *f;
    int n, m;
    char *buff = new char[LONGLONG_BITS*2];
    char *port = new char[PORT_LEN];

    f=fopen("nix/RIB2.txt","r");
    fscanf(f,"%d\n",&n);
    for(int i=0;i<n;i++){
        fscanf(f,"%[^/]/%*d %s\n", buff, port);
        router.insert(buff, port);
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
                router.find(buff)->getPort(outputHead);
                break;
            case OP_ADD:{
                fscanf(f,"%*d %s", port);
                router.insert(buff, port);
                break;
            }
            case OP_DEL:{
                fscanf(f,"%*d %*s");
                router.remove(buff);
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

    return 0;
}

