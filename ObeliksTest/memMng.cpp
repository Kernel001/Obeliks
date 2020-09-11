#include "memMng.h"

MemMng::MemMng(void)
{
}

MemMng::~MemMng()
{
}

bool MemMng::AllocMemory(void ** pMemory, unsigned long ulCountByte)
{
	*pMemory = malloc(ulCountByte);
	return true;
};

void MemMng::FreeMemory(void ** pMemory)
{
	free(pMemory);
}
