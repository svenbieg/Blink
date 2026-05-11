//===========
// SpiHost.h
//===========

#pragma once


//=======
// Using
//=======

#include "Object.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Spi {


//==========
// SPI-Host
//==========

class SpiHost: public Object
{
public:
	// Common
	virtual VOID SpiBegin(SIZE_T TxSize, SIZE_T RxSize)=0;
	virtual VOID SpiEnd()=0;
	virtual VOID SpiRead(VOID* Buffer, SIZE_T Size)=0;
	virtual VOID SpiWrite(VOID const* Buffer, SIZE_T Count)=0;
};

}}