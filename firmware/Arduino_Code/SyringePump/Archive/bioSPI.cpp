#include "bioSPI.h"

void slaveInitSPI(void) {
  // Initialize SPI pins
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);
  // Enable SPI as slave
  /* Don't have to set phase, polarity b/c
   * default works with Arduino Uno */
  //SPCR |= (1 << SPR1);      /* div 16, safer for breadboards */
  //SPCR |= (1 << MSTR);                        /* clockmaster */
  SPCR |= (1 << SPE);
}


void SPI_tradeByte(uint8_t byte) {
  SPDR = byte;                       /* SPI starts sending immediately */
  loop_until_bit_is_set(SPSR, SPIF);                /* wait until done */
                                /* SPDR now contains the received byte */
}

uint8_t SPI_readByte(void) {
  SPI_tradeByte(0);
  return (SPDR);
}

void SPI_writeByte(uint8_t byte) {
  SPI_tradeByte(byte);
}

