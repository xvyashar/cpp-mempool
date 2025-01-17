# C++ MemPool

A simple, flexible, and lightweight memory pool implementation for efficient heap memory management in C++.

## Overview
This library provides a memory pool (heap) that minimizes memory fragmentation and improves performance by reusing allocated memory. The pool is highly customizable, allowing you to set both minimum and maximum sizes for better memory management.

> [!NOTE]
> This project was created solely for learning purposes and is not intended for production use.

## Features
- Efficient memory management with a custom pool for reusing memory blocks.
- Considered Memory Alignment
- Support for both custom-sized allocations and type-specific allocations.
- Easy-to-use API for acquiring and releasing memory.

## Installation

This library is designed as a small, self-contained class with only two source files (`.h` and `.cpp`). You can either:

1. **Copy and Paste**: Copy the files from the `src` directory directly into your project.
2. **CMake Integration**: Alternatively, include it as part of a CMake project by adding it to your `CMakeLists.txt`.

To include the header file, simply add:

```cpp
#include "MemPool.h"
```

## Usage

### Constructor

The memory pool is initialized with the following constructor:

```cpp
explicit MemPool(size_t min, size_t max = 0);
```

- `min`: The minimum amount of memory to allocate initially (in bytes).
- `max`: (Optional) The maximum size the pool can grow to. If set to `0`, the pool can expand dynamically.

### Acquiring and Releasing Memory

The pool supports two types of memory allocation methods: custom-sized and type-specific.

#### 1. **Custom-Sized Allocation**

You can acquire memory of any size using the `acquire` method. The allocated memory is returned as a `void*`, which you can cast to the desired type.

Example:

```cpp
#include "MemPool.h"
#include <iostream>

int main() {
    MemPool pool(20); // Initialize pool with 20B of memory

    // Acquire 4 bytes of memory and cast it to an integer pointer
    auto iPtr = reinterpret_cast<int*>(pool.acquire(4));
    *iPtr = 7;  // Store the value 7
    std::cout << *iPtr << std::endl; // Logs: 7

    pool.release(iPtr); // Release the allocated memory
    return 0;
}
```

#### 2. **Type-Based Allocation**

Alternatively, you can acquire memory based on the type you're allocating, which simplifies the code by automatically calculating the correct size.
It takes one argument as the default value which is optional.

Example:

```cpp
#include "MemPool.h"
#include <iostream>

int main() {
    MemPool pool(20); // Initialize pool with 20B of memory

    // Acquire memory for an integer and initialize it with the value 7
    auto val = pool.tbAcquire<int>(7); 
    std::cout << *val << std::endl; // Logs: 7

    pool.release(val); // Release the allocated memory
    return 0;
}
```

### Memory Release

Both methods (`acquire` and `tbAcquire`) allocate memory from the pool. To release the memory back to the pool, use the `release` method:

```cpp
pool.release(pointer);
```

Where `pointer` is the pointer returned by either `acquire` or `tbAcquire`.

## Example

Here’s a full example combining both memory allocation methods:

```cpp
#include "MemPool.h"
#include <iostream>

int main() {
    // Create a memory pool with 100 bytes of initial space
    MemPool pool(100, 500); // Optional max size

    // Custom-sized allocation: 4 bytes for an int
    auto iPtr = reinterpret_cast<int*>(pool.acquire(4));
    *iPtr = 42;
    std::cout << "Custom allocation: " << *iPtr << std::endl;

    // Type-based allocation: Memory for an integer
    auto val = pool.tbAcquire<int>(5);
    std::cout << "Type-based allocation: " << *val << std::endl;

    // Release memory back to the pool
    pool.release(iPtr);
    pool.release(val);

    return 0;
}
```

## Contributing

Feel free to fork the repository, submit issues, and create pull requests! Contributions are always welcome.

## License
```plain
MIT License

Copyright (c) 2025 xvyashar
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```