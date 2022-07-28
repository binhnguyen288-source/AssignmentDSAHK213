#include "main.h"
#include "Cache.h"
#include "Cache.cpp"
#include "gentestcase.hpp"
using namespace std;

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

void simulate(string filename, Cache* c)
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



std::string getGot(string filename) {
    std::ostringstream buf;
    auto save = std::cout.rdbuf(buf.rdbuf());
    Cache* c = new Cache(MAXSIZE);
  	string fileName = "testcase";
    simulate(fileName, c);
  	delete c;
    std::cout.rdbuf(save);
    return buf.str();
}

#include <unordered_map>

extern "C" int signal_open(int fd, int reset);

extern "C" int __syscall_openat(int dirfd, intptr_t path, int flags, ...) {
 
   static const std::unordered_map<std::string, int> MapFile2FD{
       {"/dev/stdin", 0},
       {"/dev/stdout", 1},
       {"/dev/stderr", 2},
       {"testcase", 3},
       {"expected", 4},
       {"got", 5}
   };
   int fd = MapFile2FD.at((const char*)path);
   signal_open(fd, flags != 32768);
   return fd;
}

void printJS(const char* id, const char* str) {
    std::ofstream out(id);
    out << id << ":\n" << str;
}

int main() {
    const char* filename = "testcase";
    
    int const numtestcase = 10000;
    int i = 0;
    for (i = 0; i < numtestcase; ++i) {
        {
            std::ofstream out(filename);
            out << get_test_case();
        }
        std::string expected = ans::getExpected(filename) + "\nEnd here.";
        printJS("expected", expected.c_str());

        std::string got = getGot(filename) + "\nEnd here.";
        printJS("got", got.c_str());
        if (expected != got) {
            break;
        }
    }
    std::string result = std::to_string(i) + "/" + std::to_string(numtestcase);
    std::cout << result << std::endl;

   
    return 0;
}