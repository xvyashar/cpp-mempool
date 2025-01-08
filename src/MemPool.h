#pragma once

#include <cstddef>
#include <string>

class BadMemExpansion : public std::bad_alloc {
private:
    std::string message;

public:
    explicit BadMemExpansion(const std::string& msg) : message(msg) {}
    explicit BadMemExpansion(std::string&& msg) noexcept : message(std::move(msg)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

class MemPool {
public:
    MemPool() = delete;
    explicit MemPool(size_t min, size_t max = 0);
    ~MemPool();

    void* acquire(size_t size);

    template<typename T>
    T* acquire();

    template<typename T>
    T* acquire(T&&  defaultValue);

    void release(void* ptr);
private:
    size_t currentSize = 0;
    size_t max = 0;

    struct MemChunk {
        char* addr = nullptr;
        size_t size = 0;
        bool inuse = false;
        MemChunk* next = nullptr;
    };
    MemChunk* memChunks;
};

// template functions implementation
template<typename T>
T* MemPool::acquire() {
    return reinterpret_cast<T*>(acquire(sizeof(T)));
}

template<typename T>
T* MemPool::acquire(T&& defaultValue) {
    T* mem = reinterpret_cast<T*>(acquire(sizeof(T)));
    *mem = defaultValue;
    return mem;
}