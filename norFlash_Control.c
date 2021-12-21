#include "norFlash_Control.h"


u32 Flash_Read(u8 *_readBuffer, u32 _size, u32 _startAddress)
{
	u32 i;
	for( i = 0 ; i < _size ; i++)
	{
		_readBuffer[i] = (*((volatile U8*)_startAddress + i));
	}
	return _size;
}

u32 Flash_Write (u8* _writebuf, u32 _size,  u32 _startAddress)
{
	return norflash_write (_writebuf, _startAddress, _size);
}

u32 Flash_Erase(u32 _sector)
{
	return norflash_erase(_sector * 1024*64, 1024*64);	
}
