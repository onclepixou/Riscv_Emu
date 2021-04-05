#include "ExecutionCore.h"

#include "utils.h"
#include "Tracer.h"

#include <stdexcept>
#include <iostream>


 ExecutionCore::ExecutionCore(uint32_t* pc, uint32_t* reg) : m_pc(pc), m_reg(reg), m_mem(nullptr){

     InitExecuteI();

 }

void ExecutionCore::SetPc(uint32_t offset){

    *m_pc = offset;
}

uint32_t ExecutionCore::Pc() const{

    return *m_pc;
}

uint32_t ExecutionCore::ux(uint8_t reg) const{

    if(reg > 31){

        std::runtime_error("Register out of bound for read");
    }

    return m_reg[reg];
}

int32_t ExecutionCore::sx(uint8_t reg) const{

    if(reg > 31){

        std::runtime_error("Register out of bound for read");
    }

    return (int32_t)m_reg[reg];
}

void  ExecutionCore::x(uint8_t reg, uint32_t val){

    if(reg == 0){

        // x0 is read only
        return;
    }

    if(reg > 31){

        std::runtime_error("Register out of bound for write");
    }

    m_reg[reg] = val;
    return;
}

Memory& ExecutionCore::Mem(){

    return *m_mem;
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

            throw std::runtime_error("Found unknown instruction during execution");
            break;            

    }

    return;    
}


void ExecutionCore::I_LoadInstr(DecoderOutput executeInput){

    int32_t offset;
    uint32_t addr_fetched;
    uint32_t value_fetched;

    switch(executeInput.funct3){

        case 0x0 : 

            // LB
            offset = executeInput.SextImmI();
            addr_fetched = ux(executeInput.rs1) + offset;
            value_fetched = Mem()[addr_fetched] & 0xFF ;
            x(executeInput.rd, sext(value_fetched, 7)); // A TESTER
            break;

        case 0x1 : 

            //LH
            offset = executeInput.SextImmI();
            addr_fetched = ux(executeInput.rs1) + offset;
            value_fetched = Mem()[addr_fetched] & 0xFFFF ;
            x(executeInput.rd, sext(value_fetched, 17)); // A TESTER
            break;

        case 0x2 : 

            // LW
            offset = executeInput.SextImmI();
            addr_fetched = ux(executeInput.rs1) + offset;
            value_fetched = Mem()[addr_fetched];
            x(executeInput.rd, value_fetched);          // A TESTER
            break;

        case 0x4 : 

            // LBU
            offset = executeInput.SextImmI();
            addr_fetched = ux(executeInput.rs1) + offset;
            value_fetched = Mem()[addr_fetched] & 0xFF ;
            x(executeInput.rd, value_fetched); // A TESTER
            break;

        case 0x5 : 

            // LHU
            offset = executeInput.SextImmI();
            addr_fetched = ux(executeInput.rs1) + offset;
            value_fetched = Mem()[addr_fetched] & 0xFFFF ;
            x(executeInput.rd, value_fetched); // A TESTER
            break;

        default :

            break;
    }

    std::cout << "loaded value " << std::dec << value_fetched << std::endl;

    return;
}  

void ExecutionCore::I_FenceInstr(DecoderOutput executeInput){

    throw std::runtime_error("Fence instructions are not yet supported");
    return;
}

void ExecutionCore::I_ImmediateInstr(DecoderOutput executeInput){

    int32_t imm = executeInput.SextImmI();
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
            m_reg[executeInput.rd] = m_reg[executeInput.rs1] <<  ( executeInput.imm & 0x1F );
            break;

        case 0x5 : 

            if(shiftType == 0x0){

                // SRLI
                m_reg[executeInput.rd] = m_reg[executeInput.rs1] >>  ( executeInput.imm & 0x1F );
            }

            if(shiftType == 0x20){

                // SRAI
                m_reg[executeInput.rd] = ((int32_t)m_reg[executeInput.rs1] ) >>  ( executeInput.imm & 0x1F );
            }

            break;

        default : 

            break;
    }

    return;
}

void ExecutionCore::I_JALR(DecoderOutput executeInput){

    uint32_t t = Pc();
    int32_t offset =  executeInput.SextImmI(); 
    SetPc( ( ux( executeInput.rs1 ) + offset) &~1 );

    if(executeInput.rd == 0){

        executeInput.rd = 1;
    }

    x(executeInput.rd, t);
    return;
}   

void ExecutionCore::I_SyscallInstr(DecoderOutput executeInput){

    if(executeInput.funct3 == 0){

        if(executeInput.imm == 0 ){

            // ECALL
            std::cout << "PERFORMING ECALL" << std::endl;
            return;
        }

        if(executeInput.imm == 1){

            //EBREAK;
            std::cout << "PERFORMING EBREAK" << std::endl;
            return;
        }
    }

    else{

        throw std::runtime_error("CsrXXX instructions are not yet supported");
    }

    return;
}

void ExecutionCore::I_StoreInstr(DecoderOutput executeInput){

    int32_t offset = executeInput.SextImmI();
    uint32_t dest_addr;
    uint32_t value_stored;


    switch(executeInput.funct3){

        case 0x0 : 

            //SB
            dest_addr = ux(executeInput.rs1) + offset;
            value_stored = ux(executeInput.rs2)  & 0xFF;
            Mem()[dest_addr] = value_stored;
            break;

        case 0x1 : 

            //SH
            dest_addr = ux(executeInput.rs1) + offset;
            value_stored = ux(executeInput.rs2) & 0xFFFF;
            Mem()[dest_addr] = value_stored;
            break;

        case 0x2 : 

            //SW
            dest_addr = ux(executeInput.rs1) + offset;
            value_stored = ux(executeInput.rs2);
            Mem()[dest_addr] = value_stored;
            break;

        default : 

            break;
    }

    std::cout << "Stored value " << value_stored << " at addr " << std::hex << dest_addr << "( " <<  ux(executeInput.rs1)  << " + " << offset <<  ")" << std::endl;
    return;
} 

void ExecutionCore::I_BranchInstr(DecoderOutput executeInput){

    int32_t offset = sext(executeInput.imm, 12);

    switch(executeInput.funct3){

        case 0x0 : 

            // BEQ
            if( ux(executeInput.rs1) == ux(executeInput.rs2) ){

                SetPc(Pc() + offset - 4);
            }

            break;

        case 0x1 : 

            // BNE
            if( ux(executeInput.rs1) != ux(executeInput.rs2) ){

                SetPc(Pc() + offset - 4);
            }

            break;

        case 0x4 : 

            // BLT
            if( sx(executeInput.rs1 ) <= sx(executeInput.rs2 ) ){

                SetPc(Pc() + offset - 4);
            }

            break;

        case 0x5 : 

            // BGE
            if( sx(executeInput.rs1 ) >= sx(executeInput.rs2 ) ){

                SetPc(Pc() + offset - 4);
            }

            break;

        case 0x6 : 

            // BLTU
            if( ux(executeInput.rs1 ) <= ux(executeInput.rs2 )  ){

                SetPc(Pc() + offset - 4);
            }

            break;

        case 0x7 : 

            // BGEU
            if( ux(executeInput.rs1) >= ux(executeInput.rs2 ) ){

                SetPc(Pc() + offset - 4);
            }

            break;

        default : 

            break;
    }

    return;
}  

void ExecutionCore::I_AUIPC(DecoderOutput executeInput){

    x(executeInput.rd, Pc() + executeInput.SextImmU());
    return;
}   

void ExecutionCore::I_LUI(DecoderOutput executeInput){

    m_reg[executeInput.rd] = executeInput.imm;
    return;
}   

void ExecutionCore::I_JAL(DecoderOutput executeInput){

    int32_t offset = sext(executeInput.imm, 20);

    if(executeInput.rd == 0){
        executeInput.rd = 1;
    }

    x(executeInput.rd, Pc() + 4);
    SetPc(Pc() + offset - 4 );

    return;
}         