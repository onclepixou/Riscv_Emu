#ifndef DECODER_H
#define DECODER_H

#include "Opcodes.h"

#include <cstdint>
#include <string>
#include <map>

class Decoder{

    public : 

        Decoder();
        DecoderOutput Decode(uint32_t instr) const;
        void SetDecodeInfo(std::map <uint8_t, InstructionInfo >& info);

    private : 

        std::map <uint8_t, InstructionInfo > m_validOpcodes;

};

#endif