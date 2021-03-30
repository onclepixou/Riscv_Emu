#ifndef TRACER_H
#define TRACER_H

#include "Opcodes.h"

#include <string>
#include <cstdint>
#include <map>

class Tracer{

    public : 

        Tracer();
        void PrettyPrint(DecoderOutput decoded)const;

    private : 

        void PrintExtensionI(DecoderOutput decoded)const;
        void PrintIR(DecoderOutput decoded)const;
        void PrintII(DecoderOutput decoded)const;
        void PrintIS(DecoderOutput decoded)const;
        void PrintIB(DecoderOutput decoded)const;
        void PrintIU(DecoderOutput decoded)const;
        void PrintIJ(DecoderOutput decoded)const;

    public : 

        static const std::map<uint8_t, std::string> registerName;



};

#endif