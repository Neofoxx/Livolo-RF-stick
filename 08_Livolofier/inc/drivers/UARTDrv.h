#ifndef UARTDRV_H_a424a95491db41c6ab0c1d6e8070a876
#define UARTDRV_H_a424a95491db41c6ab0c1d6e8070a876

#include <inttypes.h>

void UARTDrv_Init(uint32_t baud);
void UARTDrv_SendBlocking(uint8_t * buffer, uint32_t length);
uint32_t UARTDrv_GetCount();

#endif
