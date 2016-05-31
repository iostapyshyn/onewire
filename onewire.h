#ifndef ONEWIRE_H
#define ONEWIRE_H

#ifndef ONEWIRE_DDR
#define ONEWIRE_DDR DDRB
#endif

#ifndef ONEWIRE_PORT 
#define ONEWIRE_PORTB
#endif

#ifndef ONEWIRE_PIN
#define ONEWIRE_PIN PINB
#endif

#ifndef ONEWIRE_PIN_NUM
#define ONEWIRE_PIN_NUM PB0
#endif

#define DS18S20_FAMILY_CODE 0x10
#define DS18B20_FAMILY_CODE 0x28
#define DS1822_FAMILY_CODE 0x22

#define onewire_low() (ONEWIRE_DDR |= (1 << ONEWIRE_PIN_NUM))
#define onewire_high() (ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM))
#define onewire_level() (ONEWIRE_PIN & (1 << ONEWIRE_PIN_NUM))
#define delay_us(x) _delay_us(x)

typedef struct onewire_bus {
    uint8_t *ddr;
    uint8_t *port;
    uint8_t *pin;
    uint8_t pin_num;
} onewire_bus;

uint8_t onewire_reset();

void onewire_send_bit(uint8_t);
void onewire_send(uint8_t);

uint8_t onewire_read_bit();
uint8_t onewire_read();

uint8_t onewire_skip();
uint8_t onewire_read_rom(uint8_t *);
uint8_t onewire_match(uint8_t *);

void onewire_enum_init();
uint8_t *onewire_enum_next();
uint8_t onewire_match_last();

uint8_t onewire_crc_update(uint8_t, uint8_t);

#endif
