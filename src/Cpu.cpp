#include "Cpu.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <stdexcept>

Cpu::Cpu(std::string extension) : m_ready(false), m_baseMemoryOffset(0), m_entryPoint(0), m_traceActivated(false), m_executionCore(&m_pc, m_registers){

    // register reset
    memset(m_registers, 0, 32 * sizeof(uint32_t));

    // load instructions
    m_validOpcodes.clear();

    for(const char& c : extension){

        if( ( c == 'I' ) || ( c == 'i' ) ){

            m_validOpcodes.insert(Opcodes::OpExtensionI.begin(), Opcodes::OpExtensionI.end());
        }

        else{

        }
    }

    m_decoder.SetDecodeInfo(m_validOpcodes);
    

}

Cpu::~Cpu(){

}

const std::map <uint8_t, InstructionInfo >& Cpu::ValidOpCodes() const{
    
    return m_validOpcodes;
}

uint32_t Cpu::x(uint8_t reg) const{

    if(reg > 31){

        throw std::out_of_range("x" + std::to_string((uint32_t)reg) + " is not a valid register");
    }

    return m_registers[reg];
}

void Cpu::AttachMemory(Memory* mem){

    m_memory = mem;
    m_executionCore.AttachMemory(mem);
    return;
}

uint32_t Cpu::VirtualtoPhysicalAddress(uint32_t addr) const{

    return addr - m_baseMemoryOffset;
}

bool Cpu::LoadElf(std::string filename){

    std::runtime_error LoadException("");

    Elf32_Ehdr header;
    std::vector<Elf32_Phdr> ProgramHeaderEntryToMap;
    Elf32_Phdr cur;

    uint32_t LastElement;
    size_t RequiredMemorySpace;

    FILE* fp = fopen(filename.c_str(), "rb");

    if(!fp){

        LoadException = std::runtime_error("Could not open elf file");
        goto LOAD_ERROR;
    }

    fread(&header, sizeof(header), 1, fp);

    if(memcmp(header.e_ident, ELFMAG, SELFMAG) != 0){

        LoadException = std::runtime_error("Invalid Elf File");
        goto LOAD_ERROR;
    }

    m_entryPoint = header.e_entry;

    fseek(fp, header.e_phoff, SEEK_SET);

    for(int i = 0; i < header.e_phnum; i++){

        fread(&cur, sizeof(Elf32_Phdr), 1, fp);

        if( ( cur.p_type == PT_LOAD) || ( cur.p_type == PT_INTERP) ){

            ProgramHeaderEntryToMap.push_back(cur);
        }

        else{

            std::cout << "INFO : unsupported program header entry type : " << cur.p_type << ". It is advised to check manually" << std::endl;
        }
    }


    if(ProgramHeaderEntryToMap.size() <= 0){

        LoadException = std::runtime_error("Nothing to map");
        goto LOAD_ERROR;
    }

    m_baseMemoryOffset = ProgramHeaderEntryToMap[0].p_vaddr;
    LastElement = ProgramHeaderEntryToMap.size() - 1;
    RequiredMemorySpace = ProgramHeaderEntryToMap[LastElement].p_offset + ProgramHeaderEntryToMap[LastElement].p_memsz;

    if(RequiredMemorySpace > m_memory->GetMemSize() ){

        LoadException = std::runtime_error("Insufficient memory space");
        goto LOAD_ERROR;
    }

    for(int i = 0; i < ProgramHeaderEntryToMap.size(); i++){

        fseek(fp, ProgramHeaderEntryToMap[i].p_offset, SEEK_SET);
        uint8_t* data = (uint8_t*)calloc(ProgramHeaderEntryToMap[i].p_memsz, sizeof(uint8_t));
        fread(data, ProgramHeaderEntryToMap[i].p_memsz, sizeof(uint8_t), fp);
        m_memory->WriteChunk(data, ProgramHeaderEntryToMap[i].p_offset, ProgramHeaderEntryToMap[i].p_memsz, sizeof(uint8_t) );
        free(data);
    }

    fclose(fp);

    m_ready = true;
    return true;

    LOAD_ERROR : 

        fclose(fp);
        m_ready = false;
        throw LoadException;

        return false;
}

void Cpu::SetTraceActivated(bool status){

    m_traceActivated = status;
    return;
}

void Cpu::Start(){

    if(m_memory == nullptr){

        throw std::runtime_error("No memory attached to the cpu");
    }

    m_pc = m_entryPoint;

    while(m_pc != 0x10544){

        // fetch
        uint32_t readAddr = VirtualtoPhysicalAddress(m_pc);
        uint32_t  nextInstr = (*m_memory)[readAddr]; 
        
        // decode
        DecoderOutput output =  m_decoder.Decode(nextInstr);

        // optional trace
        if(m_traceActivated)
            m_Tracer.PrettyPrint(output);

        // execute
        m_executionCore.Execute(output);

        // increase pc
        m_pc += 4;
    }


    return;
}
