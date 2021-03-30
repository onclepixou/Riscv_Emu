#include "Memory.h"

#include <stdexcept>
#include <cstring>

Memory::Memory(size_t memsize) : m_memory(NULL){

    if( (memsize % 4 ) != 0 ){

        throw std::runtime_error("Memory size must be a multiple of 4");
    }

    m_memoryLength = memsize;
    m_memory = (uint8_t*)calloc(m_memoryLength, sizeof(uint8_t));

    if(m_memory == NULL){

        throw std::runtime_error("Could not allocate emulated memory");
    }

}

Memory::~Memory(){

    if(m_memory != NULL){

        free(m_memory);
        m_memory = NULL;
    }

}

bool Memory::ResizeMemory(size_t bytesize){

    if(m_memory != NULL){

        free(m_memory);
        m_memory = NULL;
    }

    m_memory = (uint8_t*)calloc(bytesize, sizeof(uint8_t));
    m_memoryLength = (m_memory == NULL) ? 0 : bytesize;

    return (m_memory != 0 );
}

void Memory::EraseMemory(){

    if(m_memory != NULL){

        memset(m_memory, 0, m_memoryLength);
       
    }

}

size_t Memory::GetMemSize() const{

    return m_memoryLength;
}

uint32_t Memory::operator [] (uint32_t addr) const{

    if( (addr % 4 ) != 0 ){

        throw std::runtime_error("Only addresses that are multiple of 4 can be read");
    }

    return *((uint32_t*)(m_memory + addr));   
}

uint32_t& Memory::operator [](uint32_t addr){

    if( (addr % 4 ) != 0 ){

        throw std::runtime_error("Only addresses that are multiple of 4 can be written");
    }

    return  *((uint32_t*)(m_memory + addr));
}


bool Memory::WriteChunk(void* data, size_t base, size_t nbelem, size_t elemsize){


    if( ( base + nbelem * elemsize ) > m_memoryLength ){

        throw std::runtime_error("Cannot write data chunk to memory : not enough space");
    }

    memcpy(m_memory + base, data, nbelem * elemsize);

    return true;
}


