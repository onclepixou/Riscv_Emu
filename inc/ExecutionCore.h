#ifndef EXECUTION_CORE_H
#define EXECUTION_CORE_H

#include "Memory.h"
#include "Opcodes.h"

#include <cstdint>


class ExecutionCore{

    typedef void (ExecutionCore::*EXEC)(DecoderOutput);

    public : 

        ExecutionCore(uint32_t* pc, uint32_t* reg);
        void AttachMemory(Memory* mem);

        void Execute(DecoderOutput executeInput);

    private : 
    
        // I instructions
        void InitExecuteI();
        void I_RegisterInstr(DecoderOutput executeInput);
        void I_LoadInstr(DecoderOutput executeInput);  
        void I_FenceInstr(DecoderOutput executeInput);     
        void I_ImmediateInstr(DecoderOutput executeInput);  
        void I_JALR(DecoderOutput executeInput);            
        void I_SyscallInstr(DecoderOutput executeInput);   
        void I_StoreInstr(DecoderOutput executeInput);      
        void I_BranchInstr(DecoderOutput executeInput);     
        void I_AUIPC(DecoderOutput executeInput);           
        void I_LUI(DecoderOutput executeInput);             
        void I_JAL(DecoderOutput executeInput);     

    private : 

        uint32_t* m_pc;
        uint32_t* m_reg;
        Memory*   m_mem;

        std::map<uint8_t, EXEC> ExecuteI;

};

#endif 