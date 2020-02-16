//
// Created by tt on 2/10/20.
//
#include "cache.hpp"
#include <iostream>
using namespace std;

int main() {
    uint32_t addr = 0xFFFFFF00;
    cout << hex << addr << endl;
    cout << hex << (TAG_MASK(addr, 16, 8)) << endl;
}
