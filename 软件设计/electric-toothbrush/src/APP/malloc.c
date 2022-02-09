#include "includes.h"

Memory_ManageTableDef	mem = {
0, 0, 0, NULL, NULL,  memManageInit, mem_perUsage
};
t_block		Header;


uint8_t usedFlag[MANAGE_BLOCK_CAPACITY];			/*!< Specifies the Memory management table. */	
uint8_t Memory[MEM_CAPACITY];
struct s_block manageTable[MANAGE_BLOCK_CAPACITY];	/*!< It is the main part of memory. */

/**
		Management memory Control
**/

/**
**@name
**@func
**/
void memManageInit(void)
{
	mem.initFlag = 1;
	mem.manageTable = manageTable;
	mem.usedFlag = usedFlag;
	mem.initFunc = memManageInit;
	mem.perUsage = mem_perUsage;
	mem.usage = 0;
	
	Header = manageTable;
	
	Header->addr = Memory;
	Header->size = 0;
	Header->next = NULL;
}

void *memManageMalloc(void)
{
	uint32_t pointer = 0;
	if(mem.usedBlocks > MANAGE_BLOCK_CAPACITY)	return NULL;
	while(mem.usedFlag[pointer])
	{
		pointer ++;
	}
	mem.usedFlag[pointer] = 1;
	mem.usedBlocks ++;
	return (&mem.manageTable[pointer]);
}

void memManageFree(t_block addr)
{
	uint32_t pointer = 0;
	while(addr != (&mem.manageTable[pointer]))
	{
		pointer ++;
	}
	mem.usedBlocks -- ;
	mem.usedFlag[pointer] = 0;
}

/**
		Management memory Control
**/

/**
**@name
**@func
**@input
**@return 
**/
uint32_t mem_perUsage(void)
{
	return mem.usage;
}


void *mymalloc(uint32_t size)
{
	t_block		b;
	t_block		tmp;
	uint32_t 	capacity = 0;
	
	b = Header;
	
	if(!mem.initFlag) mem.initFunc();
	
	while(b->next != NULL)
	{
		capacity = (uint32_t)(b->next->addr) - b->size - (uint32_t)b->addr;		// The capacity between current block and next block
		if(capacity >= size)
		{
			if(b == Header)
			{
				b->size = size;
				return (b->addr);
			}
			else {
				tmp = memManageMalloc();						// distribute a memory to a new block
				tmp->addr = (void *)((uint32_t)b->addr + b->size);
				tmp->size = size;
				tmp->next = b->next;
				b->next = tmp;
				mem.usage += size;
				memset((tmp->addr), 0, size);
				return (tmp->addr);
			}
		}
		else
		{
			b = b->next;									// Update the pointer
		}
	}
	
	// There are no capacity between any two block
	// We could create a new block after confirming memory remained
	if((MEM_CAPACITY - mem.usage) > size)
	{
		if(b == Header)
		{
			b->size = size;
			return (void *)(b->addr);
		}
		else {
			tmp = memManageMalloc();						// distribute a memory to a new block
			tmp->addr = (void *)((uint32_t)b->addr + b->size);
			tmp->size = size;
			tmp->next = b->next;
			b->next = tmp;
			mem.usage += size;
			memset((void *)(tmp->addr), 0, size);
			return (void *)(tmp->addr);
		}
	}
	else 
		return NULL;
}


void myfree(void * addr)
{
	t_block b = Header;
	t_block tmp = Header;
	
	if(!mem.initFlag)	mem.initFunc();
	
	if(b->addr == addr)
	{
		mem.usage -= b->size;
		b->size = 0;
		return;
	}
	
	while(b != NULL)
	{
		if(addr == b->addr)
		{
			mem.usage -= b->size;
			tmp->next = b->next;
			memManageFree(b);
		}
		else
			tmp = b;
			b = b->next;
	}
}













