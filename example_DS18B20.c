#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "onewire.h"

#define BAUDRATE 9600UL
#define BAUD_PRESCALE (F_CPU/(16*BAUDRATE)-1)
#define HI(x) ((x) >> 8)
#define LO(x) ((x) & 0xFF)

void usart_putchar(char);
int usart_stream_handler(char, FILE *);
static FILE usart_stream = FDEV_SETUP_STREAM(usart_stream_handler, NULL, _FDEV_SETUP_WRITE);

int main() {
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

    UBRRL = LO(BAUD_PRESCALE);
    UBRRH = HI(BAUD_PRESCALE);

    stdout = &usart_stream;

    while (1) {
        if (onewire_skip()) {
            onewire_send(0x44);
    //        onewire_enum_init();
            while (1) {
                uint8_t i, scratchpad[9];
                uint8_t *p = 0; //onewire_enum_next();
                uint8_t crc = 0;
                if (p == NULL)
                    break;
                if (*p != DS18B20_FAMILY_CODE) {
                    printf("\nDevice is not DS18B20\n", *p);
                    break;
                }
                printf("[");
                for (i = 0; i < 8; i++) {
                    printf("%02X", p[i]);
                    crc = onewire_crc_update(crc, p[i]);
                }
                printf("]");
                if (crc != 0) {
                    printf("\nCRC is not valid\n");
                    break;
                }
                onewire_send(0xBE);
                crc = 0;
                for (i = 0; i < 9; i++) {
                    scratchpad[i] = onewire_read();
                    onewire_crc_update(crc, scratchpad[i]);
                }
                if (crc != 0) {
                    printf("Scratchpad CRC is not valid\n");
                    break;
                }
                uint8_t lsbyte = scratchpad[0];
                uint8_t msbyte = scratchpad[1];
                uint8_t sign = msbyte & 0b10000000;
                if (sign) {
                    uint16_t tmp = (msbyte << 8) | lsbyte;
                    tmp = ~tmp + 1;
                    lsbyte = LO(tmp);
                    msbyte = HI(tmp);
                    printf(" -");
                } else printf(" +");
                uint8_t t = ((lsbyte & 0b11110000) >> 4) | ((msbyte & 0b00000111) << 4);
                uint8_t tfract = ((lsbyte & 0b00001111) * 10) >> 4;
                printf("%d.%d °C\n", t, tfract);
            }
        }
        _delay_ms(2000);
    }
}

void usart_putchar(char data) {
    while (!(UCSRA & (1 << UDRE)));
    UDR = data;
}

int usart_stream_handler(char var, FILE *stream) {
    if (var == '\n')
        usart_putchar('\r');
    usart_putchar(var);
    return 0;
}
