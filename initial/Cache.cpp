#include "main.h"

Data* Cache::read(int addr) {
    cout << "1" << endl;
    return NULL;
}
Elem* Cache::put(int addr, Data* cont) {
    cout << "2" << endl;
    return NULL;
}
Elem* Cache::write(int addr, Data* cont) {
    cout << "3" << endl;
    return NULL;
}
void Cache::print() {
	for (int i = 0; i < p; i++)
        arr[i]->print();
}
void Cache::preOrder() {
	for (int i = 0; i < p; i++)
        arr[i]->print();
}
void Cache::inOrder() {
    for (int i = 0; i < p; i++)
        arr[i]->print();
}