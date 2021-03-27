#ifndef SPIDRV_H_c2c04c3074504f0da3fe1502cf5f02cc
#define SPIDRV_H_c2c04c3074504f0da3fe1502cf5f02cc

#include <inttypes.h>

// We do some trickery with the SPI module.
#define SPI_MODE_NORMAL			0
#define SPI_MODE_RX_OTHER_SDI	1
#define SPI_MODE_TX_OTHER_SDO	2

void SPIDrv_Init(uint32_t clkSpeed, uint8_t edge, uint32_t polarity, uint8_t sample, uint8_t mode);
void SPIDrv_SendBlocking(uint8_t * buffer, uint32_t length);
void SPIDrv_SendReceiveBlocking(uint8_t * bufferIn, uint8_t * bufferOut, uint32_t length);

#endif
