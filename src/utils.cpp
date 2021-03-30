#include "utils.h"

int32_t sext(uint32_t val, uint8_t msb){

    uint32_t mask = 1 << msb;
    uint32_t msb_val = (val & mask) >> msb;

    if(msb_val == 0){

        return val;
    }


    uint32_t pad_value = 0xFFFFFFFF << (msb + 1);
    return pad_value | val;

}