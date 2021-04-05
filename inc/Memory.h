#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>
#include <cstdint>

class Memory{

    public :
    
        // mem size must be given in bytes (multiple of 4 only)
        Memory(size_t memsize);
        ~Memory();

        bool ResizeMemory(size_t bytesize);
        
        size_t GetMemSize() const;
        uint32_t operator [] (uint32_t addr) const;
        uint32_t& operator [](uint32_t addr);

        bool WriteChunk(void* data, size_t base, size_t nbelem, size_t elemsize);

        uint32_t LastAddr() const; 

    private : 

        void EraseMemory();

    private : 

        size_t m_memoryLength;
        uint8_t* m_memory;


};

#endif
