#include "includes.h"

/**
**@name	void ReadWordFromFlash(uint32_t *src, uint32_t *dst, uint32_t length)
**@func	read a serial words from eflash
**@input	src: address of source
			dst: address of destination
			length: the length of data which should be read
**/
void ReadWordFromFlash(uint32_t *src, uint32_t *dst, uint32_t length)
{
	while(length--)
	{
		*(dst++) = *(src++);
	}
}

/**
**@name	void WriteWordToFlash(uint32_t *src, uint32_t *dst, uint32_t length)
**@func	write a serial words to eflash
**@input	src: address of source
			dst: address of destination
			length: the length of data which should be writen
**/
void WriteWordToFlash(uint32_t *src, uint32_t *dst, uint32_t length)
{
	uint32_t *page;
	uint32_t offset;
	uint32_t buffer[512];
	uint32_t i;
	
	offset = (uint32_t)dst & 0x01FFUL;			// get offset of writen address
	page = dst - offset;				// get page number of writen address
	if(offset != 0)						// first data is not writen to start address of a page
	{
		for(i=0;i<offset;i++)
		{
			buffer[i] = *(page+i);		// should store the data which won't be modified
		}
		length += offset;
	}
	
	while(length > 512)					// the writen data is more than the capacity of one page
	{
		memcpy(&buffer[offset], src, ((512-offset) * 4));	// memcpy transfer data by byte
		HAL_EFlash_ErasePage((uint32_t)page);		// erase page data
		for(i=0;i<512;i++)
		{
			HAL_EFlash_Program_Word((uint32_t)(page+i), buffer[i]);
		}
		offset = 0;						// offset update to be 0, because shuoul writen to a new page
		page += 0x200;					// page update to add 512, because shuoul writen to a new page
		length -= 512;
	}
	if(length != 0)						// there are some data which should be writen
	{
		memcpy(&buffer[offset], src, ((length-offset) * 4));
		for(i=length;i<512;i++)
		{
			buffer[i] = *(page+i);
		}
		HAL_EFlash_ErasePage((uint32_t)page);		// erase page data
		for(i=0;i<512;i++)
		{
			HAL_EFlash_Program_Word((uint32_t)(page+i), buffer[i]);
		}
	}
}



