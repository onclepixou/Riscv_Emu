#include "Decoder.h"
#include "CustomException.h"

#include <iostream>


Decoder::Decoder(){

}

DecoderOutput Decoder::Decode(uint32_t instr) const{

    uint8_t opcode =  0x0000007F & instr;

    if(m_validOpcodes.find(opcode) == m_validOpcodes.end()){

        throw DecodeException(opcode);
        return DecoderOutput();
    }

    DecoderOutput Output;
    Output.opcode = opcode;
    Output.info = m_validOpcodes.at(opcode);

    switch(Output.info.type){

        case InstructionType::TypeR :

			Output.rd     = ( 0xF80 & instr ) >> 7;
            Output.funct3 = ( 0x7000 & instr ) >> 12;
            Output.rs1    = ( 0xF8000 & instr ) >> 15;
            Output.rs2    = ( 0x1F08000 & instr ) >> 20;
            Output.funct7 = ( 0xFE008000 & instr ) >> 25;
            break;

        case InstructionType::TypeI : 

            Output.rd     = ( 0xF80 & instr ) >> 7;
            Output.funct3 = ( 0x7000 & instr ) >> 12;
            Output.rs1    = ( 0xF8000 & instr ) >> 15;
            Output.imm    = ( 0xFFF00000 & instr ) >> 20;
            break;

        case InstructionType::TypeS : 

            Output.funct3 = ( 0x7000 & instr ) >> 12;
            Output.rs1    = ( 0xF8000 & instr ) >> 15;
            Output.rs2    = ( 0x1F08000 & instr ) >> 20;
            Output.imm    = ( ( 0xFE008000 & instr ) >> 20 ) + ( ( 0xF80 & instr ) >> 7 );
            break;

		case InstructionType::TypeB : 

            Output.funct3 = ( 0x7000 & instr ) >> 12;
            Output.rs1    = ( 0xF8000 & instr ) >> 15;
            Output.rs2    = ( 0x1F08000 & instr ) >> 20;
            Output.imm    = ( ( 0x80 & instr ) << 4 ) + ( ( 0xF00 & instr ) >> 7 ) + ( ( 0x7E000000 & instr ) >> 20 ) + ( ( 0x80000000 & instr ) >> 19);
		    break;

		case InstructionType::TypeU :

            Output.rd     = ( 0xF80 & instr ) >> 7;
            Output.imm    = ( 0xFFFFF000 & instr );
			break;
				
		case InstructionType::TypeJ :
			
            Output.rd     = ( 0xF80 & instr ) >> 7;
            Output.imm    = ( 0xFF000 & instr ) + ( ( 0x100000 & instr ) >> 9 ) + ( ( 0x7FE00000 & instr ) >> 20 ) + ( ( 0x80000000 & instr ) >> 11 );
			break;
				
        default : 

            break;         
    }

    return Output;
}

void Decoder::SetDecodeInfo(std::map <uint8_t, InstructionInfo >& info){

    m_validOpcodes = info;
    return;
}