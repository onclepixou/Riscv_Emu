#ifndef CPU_H
#define CPU_H

#include "Decoder.h"
#include "Tracer.h"
#include "Memory.h"
#include "ExecutionCore.h"

// elf header
#include <elf.h>

#include <string>
#include <cstdint>
#include <vector>


class Cpu{

    public :

        Cpu(std::string Extensions);
        ~Cpu();
        void CpuState(uint32_t instr) const;

        uint32_t x(uint8_t reg) const;
        const std::map <uint8_t, InstructionInfo >& ValidOpCodes() const;
        void AttachMemory(Memory* mem);
        bool LoadElf(std::string filename);
        void SetTraceActivated(bool status);
        void Start();
        
    private : 

        uint32_t VirtualtoPhysicalAddress(uint32_t addr) const;
        void initRegisters();

    private : 

        // Known opcodes (linked to extensions declared)
        std::map <uint8_t, InstructionInfo > m_validOpcodes;

        // Registers
        uint32_t m_registers[32];
        uint32_t m_pc;

        // Main Memory
        Memory* m_memory;

       // Execution parameters
       bool m_ready;
       Elf32_Addr m_baseMemoryOffset;
       Elf32_Addr m_entryPoint;

        // Instr Decoder
        Decoder  m_decoder;

        // ExecutionCore
        ExecutionCore m_executionCore;

        // Instr Tracer
        bool m_traceActivated;
        Tracer m_Tracer;

};

#endif