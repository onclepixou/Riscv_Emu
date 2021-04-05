#include "Opcodes.h"
#include "utils.h"

InstructionInfo::InstructionInfo(){

    type = InstructionType::TypeUnknown;
    extension = ExtensionModule::ExtensionUnknown;
}

InstructionInfo::InstructionInfo(InstructionType type_, ExtensionModule extension_) {

    type = type_;
    extension = extension_;
}

DecoderOutput::DecoderOutput(){

    info = InstructionInfo();
}

int32_t DecoderOutput::SextImmI(){

    return (int32_t)sext(imm, 11);
}

int32_t DecoderOutput::SextImmJ(){

    return (int32_t)sext(imm, 20);
}

int32_t DecoderOutput::SextImmB(){

    return (int32_t)sext(imm, 12);
}

int32_t DecoderOutput::SextImmU(){

    return (int32_t)imm;
}

std::map <uint8_t, InstructionInfo > OpExtensionIList(){

    std::map <uint8_t, InstructionInfo > ExtensionI = {

        {0x33, {InstructionType::TypeR, ExtensionModule::ExtensionI} }, // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
        {0x3 , {InstructionType::TypeI, ExtensionModule::ExtensionI} }, // LB, LH, LW, LBU, LHU
        {0xF , {InstructionType::TypeI, ExtensionModule::ExtensionI} }, // FENCE, FENCE_I
        {0x13, {InstructionType::TypeI, ExtensionModule::ExtensionI} }, // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
        {0x67, {InstructionType::TypeI, ExtensionModule::ExtensionI} }, // JALR
        {0x73, {InstructionType::TypeI, ExtensionModule::ExtensionI} }, // ECALL, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI
        {0x23, {InstructionType::TypeS, ExtensionModule::ExtensionI} }, // SB, SH, SW
        {0x63, {InstructionType::TypeB, ExtensionModule::ExtensionI} }, // BEQ, BNE, BLT, BGE, BLTU, BGEU
        {0x17, {InstructionType::TypeU, ExtensionModule::ExtensionI} }, // AUIPC
        {0x37, {InstructionType::TypeU, ExtensionModule::ExtensionI} }, // LUI
        {0x6F, {InstructionType::TypeJ, ExtensionModule::ExtensionI} }  // JAL
    };

    return ExtensionI;
}


