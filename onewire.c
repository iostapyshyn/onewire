#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

uint8_t onewire_reset() {
    uint8_t c;
    onewire_low();
    delay_us(640);
    onewire_high();
    delay_us(2);
    for (c = 80; c; c--) {
        if (!onewire_level()) {
            while (!onewire_level());
            return 1;
        }
        delay_us(1);
    }
    return 0;
}

void onewire_send_bit(uint8_t bit) {
    onewire_low();
    if (bit) {
        delay_us(5);
        onewire_high();
        delay_us(90);
    } else {
        delay_us(90);
        onewire_high();
        delay_us(5);
    }
}

void onewire_send(uint8_t byte) {
    uint8_t b;
    for (b = 8; b; b--) {
        onewire_send_bit(byte & 1);
        byte >>= 1;
    }
}

uint8_t onewire_read_bit() {
    onewire_low();
    delay_us(2);
    onewire_high();
    delay_us(8);
    uint8_t r = onewire_level();
    delay_us(80);
    return r;
}

uint8_t onewire_read() {
    uint8_t i, b = 0;
    for (i = 8; i; i--) {
        b >>= 1;
        if (onewire_read_bit())
            b |= 0x80;
    }
    return b;
}

uint8_t onewire_skip() {
    if (!onewire_reset())
        return 0;
    onewire_send(0xCC);
    return 1;
}

uint8_t onewire_read_rom(uint8_t *buf) {
    uint8_t b;
    if (!onewire_reset())
        return 0;
    onewire_send(0x33);
    for (b = 0; b < 8; b++) {
        *(buf++) = onewire_read();
    }
    return 1;
}

uint8_t onewire_match(uint8_t *data) {
    uint8_t p;
    if (!onewire_reset())
        return 0;
    onewire_send(0x55);
    for (p = 0; p < 8; p++)
        onewire_send(*(data++));
    return 1;
}

uint8_t onewire_crc_update(uint8_t crc, uint8_t b) {
    uint8_t p;
    for (p = 8; p; p--) {
        crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0b10001100 : (crc >> 1);
        b >>= 1;
    }
    return crc;
}
