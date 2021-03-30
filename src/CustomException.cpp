#include "CustomException.h"

DecodeException::DecodeException(uint8_t op){

    msg_= "DECODE ERROR : OP CODE " + std::to_string(op)  + " IS UNKNOWN" ;
}

