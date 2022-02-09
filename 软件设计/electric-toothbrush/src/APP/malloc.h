#ifndef	__MALLOC_H__
#define	__MALLOC_H__

#include "ACM32F0x0.h"

#define		PAGE_NUM		15
#define		PAGE_SIZE		1024
#define		MEM_CAPACITY	PAGE_NUM * PAGE_SIZE
#define		BLOCK_NUM		MEM_CAPACITY / BLOCK_SIZE

#define		MANAGE_SIZE				4096			/*!< Managemet Table size. */
#define		MANAGE_BLOCK_SIZE		16
#define		MANAGE_BLOCK_CAPACITY	MANAGE_SIZE / MANAGE_BLOCK_SIZE


typedef struct s_block	*t_block;

struct s_block{
	void *addr;
	uint32_t size;
	t_block	next;
	uint32_t revs;							/*!< Reserved. */
};

typedef struct{	
	uint8_t initFlag;						/*!< Specifies whether the memory have been initialized. */
	uint32_t usage;
	uint32_t usedBlocks;					/*!< It is used to record how many memory blocks have been occupied. */
	uint8_t *usedFlag;						/*!< Specifies the Memory management table. */	
	struct s_block *manageTable;			/*!< It is the main part of memory. */
	void (*initFunc)(void);					/*!< Initialization */
	uint32_t (*perUsage)(void);				/*!< It is used to calculation the usage of memory and represent in percent. */
}Memory_ManageTableDef;


void memManageInit(void);
uint32_t mem_perUsage(void);

void *mymalloc(uint32_t size);
void myfree(void * addr);


#endif







