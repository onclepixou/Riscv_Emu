#include "Cpu.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <stdexcept>

#define STACK_SIZE 4 * 1024 * 1024

Cpu::Cpu(std::string extension) : m_ready(false), m_baseMemoryOffset(0), m_entryPoint(0), m_traceActivated(false), m_executionCore(&m_pc, m_registers){

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

void Cpu::CpuState(uint32_t instr) const{

    fprintf(stderr, "╔═══════╤════════════╦════════╤════════════╦════════╤════════════╦════════╤════════════╗\n");
	fprintf(stderr, "║ x0  z │ 0x%08x ║  x8 fp │ 0x%08x ║ x16 a6 │ 0x%08x ║ x24 s8 │ 0x%08x ║\n", m_registers[0], m_registers[8],  m_registers[16], m_registers[24]);
	fprintf(stderr, "║ x1 ra │ 0x%08x ║  x9 s1 │ 0x%08x ║ x17 a7 │ 0x%08x ║ x25 s9 │ 0x%08x ║\n", m_registers[1], m_registers[0],  m_registers[17], m_registers[25]);
	fprintf(stderr, "║ x2 sp │ 0x%08x ║ x10 a0 │ 0x%08x ║ x18 s2 │ 0x%08x ║ x26 s10│ 0x%08x ║\n", m_registers[2], m_registers[10], m_registers[18], m_registers[26]);
	fprintf(stderr, "║ x3 gp │ 0x%08x ║ x11 a1 │ 0x%08x ║ x19 s3 │ 0x%08x ║ x27 s11│ 0x%08x ║\n", m_registers[3], m_registers[11], m_registers[19], m_registers[27]);
	fprintf(stderr, "║ x4 tp │ 0x%08x ║ x12 a2 │ 0x%08x ║ x20 s4 │ 0x%08x ║ x28 t3 │ 0x%08x ║\n", m_registers[4], m_registers[12], m_registers[20], m_registers[28]);
	fprintf(stderr, "║ x5 t0 │ 0x%08x ║ x13 a3 │ 0x%08x ║ x21 s5 │ 0x%08x ║ x29 t4 │ 0x%08x ║\n", m_registers[5], m_registers[13], m_registers[21], m_registers[29]);
	fprintf(stderr, "║ x6 t1 │ 0x%08x ║ x14 a4 │ 0x%08x ║ x22 s6 │ 0x%08x ║ x30 t5 │ 0x%08x ║\n", m_registers[6], m_registers[14], m_registers[22], m_registers[30]);
	fprintf(stderr, "║ x7 t2 │ 0x%08x ║ x15 a5 │ 0x%08x ║ x23 s7 │ 0x%08x ║ x31 t6 │ 0x%08x ║\n", m_registers[7], m_registers[15], m_registers[23], m_registers[31]);
	fprintf(stderr, "╠═══════╪════════════╬════════╪════════════╬════════╪════════════╩════════╧════════════╣\n");
	fprintf(stderr, "║    pc │ 0x%08x ║  instr │ 0x%08x ║   mode │ machine                          ║\n", m_pc, instr);
	fprintf(stderr, "╚═══════╧════════════╩════════╧════════════╩════════╧══════════════════════════════════╝\n");

    return;
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

void Cpu::initRegisters(){

    // Register reset
    memset(m_registers, 0, 32 * sizeof(uint32_t));

    // Set stack pointer to end of memory (minus 3 to be aligned on 32 bit segment)
    m_registers[2] = m_memory->LastAddr() - 3;

    // Set PC to entry point
    m_pc = m_entryPoint;
    //m_pc = 0x10144;
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

    if(RequiredMemorySpace > ( m_memory->GetMemSize() - STACK_SIZE ) ){

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

    initRegisters();
    //m_pc = m_entryPoint;
    //m_pc = 0x10144;

    CpuState(0);

    while(m_pc != 0x10570){

        std::cout << "0x" << std::hex << m_pc << " : " ;
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


        CpuState(nextInstr);
        std::cin.get();
    }


    return;
}
