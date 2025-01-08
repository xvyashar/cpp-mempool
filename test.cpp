#include <iostream>
#include "src/MemPool.h"

using namespace std;

int main() {
    MemPool pool(20);

    auto val = pool.tbAcquire<int>(7);
    cout << *val << endl;

    pool.release(val);
    return 0;
}