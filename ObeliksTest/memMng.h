#pragma once
#define _WINDOWS
#include "../include/types.h"
#include "../include/IMemoryManager.h"

class MemMng : public IMemoryManager
{
public:
	MemMng(void);
	~MemMng();

	virtual bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte);
	virtual void ADDIN_API FreeMemory(void** pMemory);

};

