#pragma once

#include <stddef.h>
#include <stdint.h>

class Stack {
public:
    void Init(uint8_t *memory, size_t memory_size);
    void MakeSubstack(Stack *substack, size_t size);
    void Reset();
    void* Push(size_t size);
    void Append(uint8_t *memory, size_t size);

public:
    size_t m_SizeUsed;
    size_t m_SizeMax;
    uint8_t *m_Memory;
};
