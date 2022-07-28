#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <array>
#include <iomanip>
#include <wasi/api.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "main.h"

extern "C" int getentropy(void* buffer, size_t length) {
    return __wasi_random_get(static_cast<uint8_t*>(buffer), length);
}

template<int lower, int upper>
int get_random_int() {
    static std::mt19937 rng((std::random_device())());
    static std::uniform_int_distribution<int> dist(lower, upper);
    return dist(rng);
}

template<int lower, int upper>
float get_random_float() {
    static std::mt19937 rng((std::random_device())());
    static std::uniform_real_distribution<float> dist(lower, upper);
    return dist(rng);
}

std::string get_random_string(int length = 1) {
    std::string result;
    while (length--) {
        int idx = get_random_int<0, 3>();
        result.push_back('a' + idx);
    }
    return result;
}


std::string get_test_case() {
    std::ostringstream out;
    static std::array<std::string, 2> instructions{
        "W",
        "R"
    };
    static auto dist = []() {
        std::array<int, instructions.size()> dist;
        dist.fill(1);
        return std::discrete_distribution<int>(dist.begin(), dist.end());
    }();

    static std::mt19937 rng((std::random_device())());

    int n_lines = 100;
    while (n_lines--) {
        out << instructions[dist(rng)]   << ' ' 
            << get_random_int<0, 12>() << ' ' 
            << get_random_int<0, 12>() << '\n';
    }

    out << "P\nI\nE";

    return out.str();
}


namespace ans {

struct Node {
    int key;
    Elem* value;
    Node* left;
    Node* right;

    Node(int key, Elem* value) : key{key}, value{value}, left{}, right{} {}

    void cleanUp() {
        if (left) left->cleanUp();
        if (right) right->cleanUp();
        delete left; delete right;
    }
    
    void printInorder() {
        if (left) left->printInorder();
        value->print();
        if (right) right->printInorder();
    }

    void printPreorder() {
        value->print();
        if (left) left->printPreorder();
        if (right) right->printPreorder();
    }

};

struct BST {
    Node* root{};


    Node** findPtr(int key) {

        Node** it = &root;
        while (*it) {
            if (key == (*it)->key) break;
            it = key < (*it)->key ? &(*it)->left : &(*it)->right;
        }
        return it;
    }

    void insert(int key, Elem* value) {
        *findPtr(key) = new Node(key, value);
    }

    void erase(int key) {
        Node** it = findPtr(key);
        Node* replace = (*it)->left ? (*it)->left : (*it)->right;

        if ((*it)->left && (*it)->right) {
            Node** it_right = &(*it)->right;
            while ((*it_right)->left != nullptr) 
                it_right = &(*it_right)->left;
            replace = *it_right;
            *it_right = replace->right;
            replace->left = (*it)->left;
            replace->right = (*it)->right;
        }

        delete *it;
        *it = replace;

    }

    void printPreorder() {
        if (root) root->printPreorder();
    }
    void printInorder() {
        if (root) root->printInorder();
    }


    ~BST() {
        if (root) root->cleanUp();
        delete root;
    }
};



class Cache {
    Elem** arr;
    BST tree;
    int p{};
public:
    Cache(int s) : p{}, arr{new Elem*[s]} {}
    ~Cache() { delete[] arr; }
    Data* read(int addr);
    Elem* put(int addr, Data* cont);
    Elem* write(int addr, Data* cont);
    void print();
    void preOrder();
    void inOrder();
};


Data* Cache::read(int addr) {
    Node* find = *tree.findPtr(addr);
    return find ? find->value->data : nullptr;
}
Elem* Cache::put(int addr, Data* cont) {

    if (p != MAXSIZE) {
        arr[p] = new Elem(addr, cont, true);
        tree.insert(addr, arr[p++]);
        return nullptr;
    }

    Elem** dst = (addr % 2 == 0) ? &arr[0] : &arr[p - 1];
    Elem* ret = *dst;
    tree.erase(ret->addr);
    *dst = new Elem(addr, cont, true);
    tree.insert(addr, *dst);

    return ret;
}
Elem* Cache::write(int addr, Data* cont) {

    Elem* ret = NULL;
    Node* find = *tree.findPtr(addr);
    if (find != nullptr) {
        find->value->data = cont;
        find->value->sync = false;
    }
    else {
        put(addr, cont);
        (*tree.findPtr(addr))->value->sync = false;
    }
    return ret;
}
void Cache::print() {
	for (int i = p - 1; i >= 0; --i)
        arr[i]->print();
}
void Cache::preOrder() {
	tree.printPreorder();
}
void Cache::inOrder() {
    tree.printInorder();
}

Data* getData(string s) {
	stringstream ss(s);
	int idata;
    float fdata;
    if (ss >> idata) 
		return new Int(idata);
    else if (ss >> fdata)
    	return new Float(fdata);
    else if (s.compare("true") || s.compare("false"))
    	return new Bool(s.compare("true"));
    else {
		s.resize(s.size()-1);
		return new Address(stoi(s));
	}
    	
}
void simulate(string filename,Cache* c)
{
  ifstream ifs;
  ifs.open(filename, std::ifstream::in);
  string s;
  while (getline(ifs, s))
  {
    stringstream ss(s);
    string code,tmp;
    ss >> code;
    int addr;
    switch (code[0]) {
    	case 'R': //read
			    ss >> addr; 
    			Data* res;
				res = c->read(addr);
    			if (res == NULL) {
    				ss >> tmp;
    				c->put(addr,getData(tmp));
    			} else {
					cout << res->getValue() << endl;
				}
    			break;
        case 'U': //put
                ss >> addr;
                ss >> tmp;
                c->put(addr,getData(tmp));
                break;
    	case 'W': //write
                ss >> addr;
    			ss >> tmp;
    			c->write(addr,getData(tmp));
    			break;
    	case 'P': // print
    			cout << "Print stack\n";
    			c->print();
    			break;
    	case 'E': // preorder
    			cout << "Print BST in preorder\n";
    			c->preOrder();
    			break;
    	case 'I': // inorder
    			cout << "Print BST in inorder\n";
    			c->inOrder();
    			break;
    }
  }
}
std::string getExpected(string filename) {
    std::ostringstream buf;
    auto save = std::cout.rdbuf(buf.rdbuf());
    Cache* c = new Cache(MAXSIZE);
  	string fileName = "testcase";
    simulate(fileName, c);
  	delete c;
    std::cout.rdbuf(save);
    std::cout.rdbuf(save);
    return buf.str();
}
}