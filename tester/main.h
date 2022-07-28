#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std; 
#define MAXSIZE 15
class Data {
	public:
	virtual string getValue() = 0;
	virtual ~Data() {};
};

class Int: public Data {
	int value;
	public:
	Int(int v):value(v){}
	~Int(){}
	string getValue() {
		return to_string(value);
	}
};
class Float: public Data {
	float value;
	public:
	Float(float v):value(v){}
	~Float() {}
	string getValue() {
		return to_string(value);
	}
};
class Bool:public Data {
	bool value;
	public:
	Bool(bool v):value(v){}
	~Bool() {}
	string getValue() {
		return value?"true":"false";
	}
};
class Address:public Data {
	int value;
	public:
	Address(int v):value(v){}
	~Address() {}
	string getValue() {
		return to_string(value)+"A";
	}
};

class Elem{
public:
    int addr;
    Data* data;
    bool sync;
    Elem(int a, Data* d, bool s):addr(a), data(d), sync(s){};
    ~Elem() {delete data;}
    void print() {
        cout << addr << " " << data->getValue() << " " << (sync?"true":"false") << endl;
    }
};

#endif