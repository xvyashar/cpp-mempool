#include "MemPool.h"

#include <cstddef>
#include <string>
#include <stdexcept>

MemPool::MemPool(size_t min, size_t max): currentSize(min), max(max) {
    memChunks = new MemChunk{.addr = new char[min], .size = min};

    if (max && max < min)
        throw std::invalid_argument("max < min!");
}

MemPool::~MemPool() {
    auto* current = memChunks;
    do {
        auto* next = current->next;
        delete current;
        current = next;
    } while (current != nullptr);
}

void* MemPool::acquire(size_t size) {
    MemChunk* previous = nullptr;
    auto* current = memChunks;
    do {
        if (current->inuse) {
            previous = current;
            current = current->next;
            continue;
        }

        // available chunk scan
        if (current->size == size) {
            current->inuse = true;
            return static_cast<void*>(current->addr);
        }

        // splittable chunk scan
        if (current->size > size) {
            auto* newChunk = new MemChunk{
                    .addr = current->addr + size,
                    .size = current->size - size,
                    .next = current->next
            };

            current->size = size;
            current->inuse = true;
            current->next = newChunk;

            return static_cast<void*>(current->addr);
        }

        previous = current;
        current = current->next;
    } while (current != nullptr);

    // not available
    if (previous) {
        if (!max || (currentSize + size <= max)) {
            current = new MemChunk{.addr = new char[size], .size = size, .inuse = true};
            previous->next = current; // add to mem list
            currentSize += size;
            return static_cast<void*>(current->addr);
        } else {
            throw BadMemExpansion("exceeded to the max limit of pool!");
        }
    }

    throw std::bad_alloc();
}

void MemPool::release(void* ptr) {
    MemChunk* previous = nullptr;
    auto* current = memChunks;
    do {
        if (current->addr != ptr) {
            previous = current;
            current = current->next;
            continue;
        }

        if (previous) {
            // should not be inuse and far from current chunk
            if (!previous->inuse && previous->addr + previous->size == current->addr) {
                // merge with previous chunk
                previous->size += current->size;
                previous->next = current->next;

                delete current;
                break;
            }
        }

        current->inuse = false; //release

        if (current->next) {
            // should not be inuse and far from current chunk
            if (!current->next->inuse && current->addr + current->size == current->next->addr) {
                // merge with next chunk
                current->size += current->next->size;

                MemChunk* garbage = current->next;
                current->next = current->next->next;

                delete garbage;
                break;
            }
        }

        previous = current;
        current = current->next;
    } while (current != nullptr);
}