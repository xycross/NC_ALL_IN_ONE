#pragma once

#include "cantus.h"
#include "globalDefine.h"

u32 Flash_Read(u8 *_readBuffer, u32 _size, u32 _startAddress);
u32 Flash_Write (u8* _writebuf, u32 _size,  u32 _startAddress);
u32 Flash_Erase(u32 _sector);
