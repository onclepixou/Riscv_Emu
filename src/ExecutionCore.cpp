#include "ExecutionCore.h"

#include "utils.h"
#include <stdexcept>

 ExecutionCore::ExecutionCore(uint32_t* pc, uint32_t* reg) : m_pc(pc), m_reg(reg), m_mem(nullptr){

     InitExecuteI();

 }

 void ExecutionCore::AttachMemory(Memory* mem){

     m_mem = mem;
     return;
 }

 void ExecutionCore::Execute(DecoderOutput executeInput){

    EXEC fp;

    switch(executeInput.info.extension){

        case ExtensionModule::ExtensionI : 

            fp = ExecuteI[executeInput.opcode];
            (this->*fp)(executeInput);
            break;    

        default :

            std::runtime_error("Execution of this operation is not yet supported");
            break;
    }

 }


/* Execution of I extension instructions */
void ExecutionCore::InitExecuteI(){

    ExecuteI.clear();

    ExecuteI.insert(std::make_pair(0x33, &ExecutionCore::I_RegisterInstr  )); // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
    ExecuteI.insert(std::make_pair(0x3 , &ExecutionCore::I_LoadInstr      )); // LB, LH, LW, LBU, LHU
    ExecuteI.insert(std::make_pair(0xF , &ExecutionCore::I_FenceInstr     )); // FENCE, FENCE_I
    ExecuteI.insert(std::make_pair(0x13, &ExecutionCore::I_ImmediateInstr )); // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
    ExecuteI.insert(std::make_pair(0x67, &ExecutionCore::I_JALR           )); // JALR
    ExecuteI.insert(std::make_pair(0x73, &ExecutionCore::I_SyscallInstr   )); // ECALL, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI
    ExecuteI.insert(std::make_pair(0x23, &ExecutionCore::I_StoreInstr     )); // SB, SH, SW
    ExecuteI.insert(std::make_pair(0x63, &ExecutionCore::I_BranchInstr    )); // BEQ, BNE, BLT, BGE, BLTU, BGEU
    ExecuteI.insert(std::make_pair(0x17, &ExecutionCore::I_AUIPC          )); // AUIPC
    ExecuteI.insert(std::make_pair(0x37, &ExecutionCore::I_LUI            )); // LUI
    ExecuteI.insert(std::make_pair(0x6F, &ExecutionCore::I_JAL            )); // JAL


    return;
}


void ExecutionCore::I_RegisterInstr(DecoderOutput executeInput){

    switch(executeInput.funct3){

        case 0x0 : 

            if(executeInput.imm == 0x0){

                // ADD
                m_reg[executeInput.rd] = m_reg[executeInput.rs1] + m_reg[executeInput.rs2];
            }

            else if(executeInput.imm == 0x20){

                // SUB
                m_reg[executeInput.rd] = m_reg[executeInput.rs1] - m_reg[executeInput.rs2];
            }

            break;

        case 0x1 :

            // SLL
            m_reg[executeInput.rd] =  m_reg[executeInput.rs1] << m_reg[executeInput.rs2];
            break;

        case 0x2 : 

            // SLT
            m_reg[executeInput.rd] =  ( ( (int32_t)m_reg[executeInput.rs1] - (int32_t)m_reg[executeInput.rs2] ) > 0 ) ? 0 : 1;
            break;

        case 0x3 : 

            // SLTU
            m_reg[executeInput.rd] =  ( ( m_reg[executeInput.rs1] - m_reg[executeInput.rs2] ) > 0 ) ? 0 : 1;
            break;

        case 0x4 : 

            // XOR
            m_reg[executeInput.rd] =  m_reg[executeInput.rs1] ^ m_reg[executeInput.rs2];
            break;

        case 0x5 : 

            if(executeInput.imm == 0x0){

                // SRL
                m_reg[executeInput.rd] =  m_reg[executeInput.rs1] >> (m_reg[executeInput.rs2] & 0x1F);
            }

            if(executeInput.imm == 0x20){

                // SRA
                m_reg[executeInput.rd] =  ((int32_t)m_reg[executeInput.rs1]) >> ( (int32_t)(m_reg[executeInput.rs2]) & 0x1F );
            }

            break;

        case 0x6 : 

            // OR
            m_reg[executeInput.rd] =  m_reg[executeInput.rs1] | m_reg[executeInput.rs2];
            break;

        case 0x7 : 

            // AND
            m_reg[executeInput.rd] =  m_reg[executeInput.rs1] & m_reg[executeInput.rs2];
            break;

        default : 

            throw std::runtime_error("Found unknown instruction dureing execution");
            break;            

    }

    return;    
}


void ExecutionCore::I_LoadInstr(DecoderOutput executeInput){

    switch(executeInput.funct3){

        case 0x0 : 

            // LB
            //uint32_t addressToFetch = m_reg[executeInput.rs1] + sext(executeInput.imm & 0xFF , 7);
            break;

        case 0x1 : 

            //LH
            break;

        case 0x2 : 

            // LW
            break;

        case 0x4 : 

            // LBU
            break;

        case 0x5 : 

            // LHU
            break;

        default :

            break;
    }

    return;
}  

void ExecutionCore::I_FenceInstr(DecoderOutput executeInput){

    throw std::runtime_error("Fence instructions are not yet supported");
    return;
}

void ExecutionCore::I_ImmediateInstr(DecoderOutput executeInput){

    int32_t imm = sext(executeInput.imm, 11);
    uint8_t shiftType = executeInput.imm >> 5;

    switch(executeInput.funct3){

        case 0x0 : 

            // ADDI
            m_reg[executeInput.rd] = (int32_t)m_reg[executeInput.rs1] + imm;
            break;

        case 0x2 : 

            // SLTI
            m_reg[executeInput.rd] = ( ( (int32_t)m_reg[executeInput.rs1] - imm ) > 0 ) ? 0 : 1;
            break;

        case 0x3 : 

            // SLTIU
            m_reg[executeInput.rd] = ( ( m_reg[executeInput.rs1] - imm ) > 0 ) ? 0 : 1;
            break;

        case 0x4 : 

            // XORI
            m_reg[executeInput.rd] = m_reg[executeInput.rs1] ^ imm;
            break;

        case 0x6 : 

            // ORI
            m_reg[executeInput.rd] = m_reg[executeInput.rs1] | imm;
            break;

        case 0x7 : 

            // ANDI
            m_reg[executeInput.rd] = m_reg[executeInput.rs1] & imm;
            break;

        case 0x1 : 

            // SLLI
            m_reg[executeInput.rd] = m_reg[executeInput.rs1] <<  ( imm & 0x1F );
            break;

        case 0x5 : 

            if(shiftType == 0x0){

                // SRLI
                m_reg[executeInput.rd] = m_reg[executeInput.rs1] >>  ( imm & 0x1F );
            }

            if(shiftType == 0x20){

                // SRAI
                m_reg[executeInput.rd] = ((int32_t)m_reg[executeInput.rs1] ) >>  ( imm & 0x1F );
            }

            break;

        default : 

            break;
    }

    return;
}

void ExecutionCore::I_JALR(DecoderOutput executeInput){

    return;
}   

void ExecutionCore::I_SyscallInstr(DecoderOutput executeInput){

    return;
}

void ExecutionCore::I_StoreInstr(DecoderOutput executeInput){

    return;
} 

void ExecutionCore::I_BranchInstr(DecoderOutput executeInput){

    return;
}  

void ExecutionCore::I_AUIPC(DecoderOutput executeInput){

    return;
}   

void ExecutionCore::I_LUI(DecoderOutput executeInput){

    return;
}   

void ExecutionCore::I_JAL(DecoderOutput executeInput){

    return;
}         