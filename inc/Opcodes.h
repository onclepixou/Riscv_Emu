#ifndef OPCODES_H
#define OPCODES_H

#include <map>

enum InstructionType{

    TypeR = 0,
    TypeI = 1,
    TypeS = 2,
    TypeB = 3,
    TypeU = 4,
    TypeJ = 5,
    TypeUnknown = 6
};

enum ExtensionModule{

    ExtensionI = 0,
    ExtensionUnknown = 1
};

struct InstructionInfo{

    InstructionType type;
    ExtensionModule extension;

    InstructionInfo();
    InstructionInfo(InstructionType type_, ExtensionModule extension_);
};

struct DecoderOutput{

    InstructionInfo info;
    uint8_t  opcode;
    uint8_t  rd;
    uint8_t  rs1;
    uint8_t  rs2;
    uint8_t  funct3;
    uint8_t  funct7;
    uint32_t imm;

    DecoderOutput();

    int32_t SextImmI();
    int32_t SextImmJ();
    int32_t SextImmB();
    int32_t SextImmU();
};

// List of operations per extension
std::map <uint8_t, InstructionInfo > OpExtensionIList();

namespace Opcodes{

    const std::map <uint8_t, InstructionInfo > OpExtensionI = OpExtensionIList();

}


#endif