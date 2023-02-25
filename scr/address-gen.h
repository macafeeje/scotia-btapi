#pragma once

// Compute the MODBUS RTU CRC           https://ctlsys.com/support/how_to_compute_the_modbus_rtu_message_crc/
#include <stdint.h>
#include <iostream>
#include <string.h>
#include "config.h"

uint16_t ModRTU_CRC(uint8_t buf[], int len, bool swap_order)
{
    uint16_t crc = 0xFFFF;

    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) {    // Loop over each bit
            if ((crc & 0x0001) != 0) {      // If the LSB is set
                crc >>= 1;                    // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else                            // Else LSB is not set
                crc >>= 1;                    // Just shift right
        }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    //order is swapped here...

    //swap bits
    if (swap_order) {
        uint8_t MSB = (crc >> 8) & 0xff;
        uint8_t LSB = (crc >> 0) & 0xff;

        //recombine
        crc = (LSB << 8) | MSB;
    }

    return crc;
}

uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    return ModRTU_CRC(buf, len, true);
}

void word_to_byte(uint8_t& hi, uint8_t& lo, uint16_t _word) {
    hi = (_word >> 8) & 0xff;
    lo = (_word >> 0) & 0xff;
}

uint16_t byte_to_word(uint8_t hi, uint8_t lo) {
    return hi * 256 + lo;
}

void byte_to_char(char* output, uint8_t _byte) {
    //special format without 0x suffix
    char o[3] = { 0 };
#ifdef RPI
    sprintf(o, "%02x", (int)_byte);
    strcat(output, o);
#else
    sprintf_s(o, "%02x", (int)_byte);       // http://www.pixelbeat.org/programming/gcc/format_specs.html
    strcat_s(output, 17, o);
#endif
}

void word_to_char(char* output, uint16_t _hex) {
    uint8_t hi = 0;
    uint8_t lo = 0;
    word_to_byte(hi, lo, _hex);
    byte_to_char(output, hi);
    byte_to_char(output, lo);
}

uint8_t char_to_byte(char hi_chr, char lo_chr) {
    uint8_t hi = 0;
    uint8_t lo = 0;

    //check if 0 - 9 or a - f and convert the low order nibble accordingly
    if (lo_chr >= 48 && lo_chr <= 57) {
        lo = (lo_chr - 48);
    }else{
        lo = (lo_chr - 87);
    }

    if (hi_chr >= 48 && hi_chr <= 57) {
        hi = (hi_chr - 48) * 16;
    }else{
        hi = (hi_chr - 87) * 16;
    }

    return lo + hi;
}

//takes in a specific address to read from and generates the command
//n_to_read is number of words, e.g. 0x0001 returns two bytes
void address_gen(char *output, uint16_t preamble, uint16_t start_register, uint16_t n_to_read) {
    //16 chars exc. null
    word_to_char(output, preamble);
    word_to_char(output, start_register);
    word_to_char(output, n_to_read);

    //calc crc
    uint8_t preamble_h = 0;
    uint8_t preamble_l = 0;
    word_to_byte(preamble_h, preamble_l, preamble);

    uint8_t sr_h = 0;
    uint8_t sr_l = 0;
    word_to_byte(sr_h, sr_l, start_register);

    uint8_t n_h = 0;
    uint8_t n_l = 0;
    word_to_byte(n_h, n_l, n_to_read);

    uint8_t data[6] = { preamble_h, preamble_l, sr_h, sr_l, n_h, n_l };
    uint16_t crc = ModRTU_CRC(data, 6);
    word_to_char(output, crc);
}
