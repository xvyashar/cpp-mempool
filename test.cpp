#include <iostream>
#include <cstring>
#include "src/MemPool.h"

using namespace std;

int main() {
    MemPool pool(20);

    auto val = pool.acquire<char[12]>();
    strncpy(reinterpret_cast<char*>(val), "hello", 12);

    cout << *val << endl;

    pool.release(val);

    pool.printChunks();
    return 0;
}