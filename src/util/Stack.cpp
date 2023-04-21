#include <util/Stack.h>

#include <string.h>
#include <assert.h>

void Stack::Init(uint8_t *memory, size_t memory_size) {
    m_SizeUsed = 0;
    m_SizeMax = memory_size;
    m_Memory = memory;
}

void Stack::MakeSubstack(Stack *substack, size_t size) {
    uint8_t *memory = (uint8_t*)Push(size);
    substack->m_SizeUsed = 0;
    substack->m_SizeMax = size;
    substack->m_Memory = memory;
}

void Stack::Reset() {
    m_SizeUsed = 0;
}

void *Stack::Push(size_t size) {
    assert(m_SizeUsed + size <= m_SizeMax);

    void *result = (uint8_t*)m_Memory + m_SizeUsed;

    m_SizeUsed += size;
    return result;
}

void Stack::Append(uint8_t *memory, size_t size) {
    assert(m_SizeUsed + size <= m_SizeMax);

    void *dest = (uint8_t*)m_Memory + m_SizeUsed;

    memcpy(dest, memory, size);
    m_SizeUsed += size;
}
