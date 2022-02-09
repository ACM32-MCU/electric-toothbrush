#ifndef	__FLASH_H__
#define	__FLASH_H__

#include "ACM32F0x0.h"

#define	STORE_ADDR		(EFLASH_BASE + 0x00010000UL)


void ReadWordFromFlash(uint32_t *src, uint32_t *dst, uint32_t length);
void WriteWordToFlash(uint32_t *src, uint32_t *dst, uint32_t length);


#endif



