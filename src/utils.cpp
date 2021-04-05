#include "utils.h"

uint32_t sext(uint32_t val, uint8_t msb){

    uint32_t mask = 1 << msb;
    uint32_t msb_val = (val & mask) >> msb;
    uint32_t pad_value = 0xFFFFFFFF << (msb + 1);
    uint32_t ret = (msb_val == 0 ) ? val : (pad_value | val);

    return ret;

}