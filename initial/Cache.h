#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class Cache {
    Elem** arr;
    int p;
public:
    Cache(int s) {
        arr = new Elem*[s];
        p = 0;
    }
    ~Cache() {
        delete[] arr;
    }
    Data* read(int addr);
    Elem* put(int addr, Data* cont);
    Elem* write(int addr, Data* cont);
    void print();
    void preOrder();
    void inOrder();
};

#endif