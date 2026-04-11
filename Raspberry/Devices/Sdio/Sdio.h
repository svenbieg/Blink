//========
// Sdio.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Sdio {


//===========
// Functions
//===========

enum SDIO_FN: BYTE
{
FN0,
FN1,
FN2
};

const UINT FN0_BIT=(1<<0);
const UINT FN1_BIT=(1<<1);
const UINT FN2_BIT=(1<<2);


//==========
// Register
//==========

class SDIO_REG
{
public:
	SDIO_REG(UINT Address, SDIO_FN Function, BYTE Size): Address(Address), Function(Function), Size(Size) {}
	UINT Address;
	SDIO_FN Function;
	BYTE Size;
};


//===========
// Registers
//===========

class FN0_08: public SDIO_REG { public: FN0_08(SIZE_T Address): SDIO_REG(Address, FN0, 1) {} };
class FN0_16: public SDIO_REG { public: FN0_16(SIZE_T Address): SDIO_REG(Address, FN0, 2) {} };
class FN0_32: public SDIO_REG { public: FN0_32(SIZE_T Address): SDIO_REG(Address, FN0, 4) {} };

class FN1_08: public SDIO_REG { public: FN1_08(SIZE_T Address): SDIO_REG(Address, FN1, 1) {} };
class FN1_16: public SDIO_REG { public: FN1_16(SIZE_T Address): SDIO_REG(Address, FN1, 2) {} };
class FN1_32: public SDIO_REG { public: FN1_32(SIZE_T Address): SDIO_REG(Address, FN1, 4) {} };

}}