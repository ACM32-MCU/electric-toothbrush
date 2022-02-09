#ifndef	__DEBUG_H__
#define	__DEBUG_H__

#include	"config.h"

/**
	This define some debug printf function
**/
#ifdef	DEBUG_MODE

    #define DEBUG_PRINT    printf
	
	#define KEY_DEBUG
	#define	BLE_DEBUG

#else
    #define DEBUG_PRINT(format, ...)    ((void)0)

#endif	/*	DEBUG_MODE	*/


#ifdef	KEY_DEBUG
	#define	DEBUG_KEY	printf
#else
	#define	DEBUG_KEY(format, ...)    ((void)0)
#endif	/*	KEY_DEBUG	*/


#ifdef	BLE_DEBUG
	#define	DEBUG_BLE	printf
#else
	#define	DEBUG_BLE(format, ...)    ((void)0)
#endif	/*	BLE_DEBUG	*/

#endif





