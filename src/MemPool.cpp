#include "MemPool.h"

#include <cstddef>
#include <string>
#include <stdexcept>
#include <bit>

#include <iostream>

using std::invalid_argument, std::bad_alloc, std::has_single_bit, std::cout, std::endl, std::boolalpha;

MemPool::MemPool(size_t min, size_t max): currentSize(min), max(max) {
    memChunks = new MemChunk{.rawAddr = new char[min], .size = min};

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

        size_t padding = getAlignmentPadding(current->rawAddr, size);
        size_t newSize = size + padding;

        if (current->size < newSize) {
            previous = current;
            current = current->next;
            continue;
        } else if (current->size == newSize) {
            current->inuse = true;
            current->alignedAddr = current->rawAddr + padding;
            return static_cast<void*>(current->alignedAddr);
        } else {
            auto* newChunk = new MemChunk{
                    .rawAddr = current->rawAddr + newSize,
                    .size = current->size - newSize,
                    .next = current->next
            };

            current->alignedAddr = current->rawAddr + padding;
            current->size = newSize;
            current->inuse = true;
            current->next = newChunk;

            return static_cast<void*>(current->alignedAddr);
        }
    } while (current != nullptr);

    // not available
    if (previous) {
        if (!max || (currentSize + size <= max)) {
            current = new MemChunk{.rawAddr = reinterpret_cast<char*>(malloc(size)), .size = size, .inuse = true}; // use malloc to get memory aligned with requested size
            current->alignedAddr = current->rawAddr; // aligned already
            previous->next = current; // add to mem list
            currentSize += size;
            return static_cast<void*>(current->rawAddr);
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
        if (current->alignedAddr != ptr) {
            previous = current;
            current = current->next;
            continue;
        }

        if (previous) {
            // should not be inuse and far from current chunk
            if (!previous->inuse && previous->rawAddr + previous->size == current->rawAddr) {
                // merge with previous chunk
                previous->size += current->size;
                previous->next = current->next;

                delete current;
                current = previous;
            }
        }

        current->inuse = false; //release

        if (current->next) {
            // should not be inuse and far from current chunk
            if (!current->next->inuse && current->rawAddr + current->size == current->next->rawAddr) {
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

void MemPool::monitPool() {
    auto* current = memChunks;
    cout << "============= MemPool Monit =============" << endl;
    size_t i = 0;
    do {
        cout << "Chunk (" << i << "):\n";
        cout << "  - chunkAddr: " << current << "\n";
        cout << "  - rawAddr: " << static_cast<void*>(current->rawAddr) << "\n";
        cout << "  - alignedAddr: " << static_cast<void*>(current->alignedAddr) << "\n";
        if (current->inuse) cout << "  - raw & aligned gap: " << reinterpret_cast<size_t>(current->alignedAddr) - reinterpret_cast<size_t>(current->rawAddr) << "\n";
        cout << "  - size: " << current->size << "\n";
        cout << "  - inuse: " << boolalpha << current->inuse << "\n";
        cout << "  - next: " << current->next << "\n";
        cout << "-----------------------------------------" << endl;

        current = current->next;
        i++;
    } while (current != nullptr);
}

// private functions
size_t MemPool::getAlignmentPadding(const void* addr, size_t alignment) {
    if (!has_single_bit(alignment)) {
        return 0;
    }

    size_t raw = reinterpret_cast<size_t>(addr);
    size_t aligned = (raw + alignment - 1) & ~(alignment - 1);
    return aligned - raw;
}