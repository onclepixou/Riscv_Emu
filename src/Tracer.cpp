#include "Tracer.h"
#include "utils.h"

#include <iostream>


const std::map<uint8_t, std::string> Tracer::registerName = {

    {0,  "zero"},
    {1,  "ra"},
    {2,  "sp"},
    {3,  "gp"},
    {4,  "tp"},
    {5,  "t0"},
    {6,  "t1"},
    {7,  "t2"},
    {8,  "fp"},
    {9,  "s1"},
    {10, "a0"},
    {11, "a1"},
    {12, "a2"},
    {13, "a3"},
    {14, "a4"},
    {15, "a5"},
    {16, "a6"},
    {17, "a7"},
    {18, "s2"},
    {19, "s3"},
    {20, "s4"},
    {21, "s5"},
    {22, "s6"},
    {23, "s7"},
    {24, "s8"},
    {25, "s9"},
    {26, "s10"},
    {27, "s11"},
    {28, "t3"},
    {29, "t4"},
    {30, "t5"},
    {31, "t6"}
};

Tracer::Tracer(){

}

void Tracer::PrettyPrint(DecoderOutput decoded)const{

    switch(decoded.info.extension){

       case ExtensionModule::ExtensionI :

            PrintExtensionI(decoded);
            break;
				
        default : 

            break;         

    }

    return;
}


void Tracer::PrintExtensionI(DecoderOutput decoded)const{

    switch(decoded.info.type){

        case InstructionType::TypeR :

            PrintIR(decoded);
            break;

        case InstructionType::TypeI : 

            PrintII(decoded);
            break;

        case InstructionType::TypeS : 

            PrintIS(decoded);
            break;

		case InstructionType::TypeB : 

            PrintIB(decoded);
		    break;

		case InstructionType::TypeU :

            PrintIU(decoded);
			break;
				
		case InstructionType::TypeJ :
			
            PrintIJ(decoded);
			break;
				
        default : 

            break;         
    }

    return;
}

std::map<uint16_t, std::string> IR_mnemonics = {

    {0x0, "ADD"},
    {0x100, "SUB"},
    {0x1, "SLL"},
    {0x2, "SLT"},
    {0x3, "SLTU"},
    {0x4, "XOR"},
    {0x5, "SRL"},
    {0x105, "SRA"},
    {0x6, "OR"},
    {0x7, "AND"}
};

void Tracer::PrintIR(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    uint16_t token = ( decoded.funct7 << 3 )  + decoded.funct3; 
    if(IR_mnemonics.find(token) != IR_mnemonics.end()){

        mnemonic = IR_mnemonics.at(token);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rd) << ", " << registerName.at(decoded.rs1) << ", " << registerName.at(decoded.rs2) << std::endl;

    return;
}


std::map<uint16_t, std::string> II_mnemonics = {

    {0x67,  "JALR"},
    {0x3,   "LB"},
    {0x83,  "LH"},
    {0x103, "LW"},
    {0x203, "LBU"},
    {0x283, "LHU"},
    {0x13,  "ADDI"},
    {0x113, "SLTI"},
    {0x193, "SLTIU"},
    {0x213, "XORI"},
    {0x313, "ORI"},
    {0x393, "ANDI"},
    {0x93,  "SLLI"},
    {0x293, "RIGHT_SHIFT"},
    {0xF,   "FENCE"},
    {0x8F,  "FENCE.I"},
    {0x73,  "ECALL_EBREAK"},
    {0xF3,  "CSRRW"},
    {0x173, "CSRRS"},
    {0x1F3, "CSRRC"},
    {0x2F3, "CSRRWI"},
    {0x373, "CSRRSI"},
    {0x3F3, "CSRRCI"}
};

void Tracer::PrintII(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    uint16_t token = decoded.opcode + ( decoded.funct3 << 7 ); 

    if(II_mnemonics.find(token) != II_mnemonics.end()){

        mnemonic = II_mnemonics.at(token);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rd) << ", " << registerName.at(decoded.rs1) << ", " << sext(decoded.imm, 11) << std::endl;
    return;
}

std::map<uint8_t, std::string> IS_mnemonics = {

    {0x0, "SB"},
    {0x1, "SH"},
    {0x2, "SW"}
};

void Tracer::PrintIS(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    if(IS_mnemonics.find(decoded.funct3) != IS_mnemonics.end()){

        mnemonic = IS_mnemonics.at(decoded.funct3);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rs2) << ", " << sext(decoded.imm, 11) << "(" << registerName.at(decoded.rs1) << ")" <<  std::endl;
    return;
}


std::map<uint8_t, std::string> IB_mnemonics = {

    {0x0, "BEQ"},
    {0x1, "BNE"},
    {0x4, "BLT"},
    {0x5, "BGE"},
    {0x6, "BLTU"},
    {0x7, "BGEU"},  
};

void Tracer::PrintIB(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    if(IB_mnemonics.find(decoded.funct3) != IB_mnemonics.end()){

        mnemonic = IB_mnemonics.at(decoded.funct3);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rs1) << ", " << registerName.at(decoded.rs2) << ", " << sext(decoded.imm, 12) <<  std::endl;
    return;
}

std::map<uint8_t, std::string> IU_mnemonics = {

    {0x17, "AUIPC"},
    {0x37, "LUI"}
};

void Tracer::PrintIU(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    if(IU_mnemonics.find(decoded.opcode) != IU_mnemonics.end()){

        mnemonic = IU_mnemonics.at(decoded.opcode);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rd) << ", " << (int32_t)decoded.imm <<  std::endl;
    return;
}

std::map<uint8_t, std::string> IJ_mnemonics = {

    {0x6F, "JAL"}
};

void Tracer::PrintIJ(DecoderOutput decoded)const{

    std::string mnemonic = "UNKNOWN";

    if(IJ_mnemonics.find(decoded.opcode) != IJ_mnemonics.end()){

        mnemonic = IJ_mnemonics.at(decoded.opcode);
    }

    std::cout << mnemonic << " " << registerName.at(decoded.rd) << ", " << sext(decoded.imm, 20) <<  std::endl;
    return;
}
