#include <util/MemArena.hpp>

#include <string.h>
#include <assert.h>

MemArena::MemArena(uint8_t *memory, size_t memory_size) {
    m_SizeUsed = 0;
    m_SizeMax = memory_size;
    m_Memory = memory;
}

void MemArena::MakeSubstack(MemArena *substack, size_t size) {
    uint8_t *memory = (uint8_t*)Push(size);
    substack->m_SizeUsed = 0;
    substack->m_SizeMax = size;
    substack->m_Memory = memory;
}

void MemArena::Reset() {
    m_SizeUsed = 0;
}

void *MemArena::Push(size_t size) {
    assert(m_SizeUsed + size <= m_SizeMax);

    void *result = (uint8_t*)m_Memory + m_SizeUsed;

    m_SizeUsed += size;
    return result;
}

void MemArena::Append(uint8_t *memory, size_t size) {
    assert(m_SizeUsed + size <= m_SizeMax);

    void *dest = (uint8_t*)m_Memory + m_SizeUsed;

    memcpy(dest, memory, size);
    m_SizeUsed += size;
}
