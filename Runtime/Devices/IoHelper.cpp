//==============
// IoHelper.cpp
//==============

#include "IoHelper.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Timers/SystemTimer.h"

using namespace Concurrency;
using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Devices {


//========
// Common
//========

VOID IoHelper::Retry(RO32& reg, UINT mask, UINT value, UINT retry)
{
for(UINT u=0; u>=0; u++)
	{
	if(IoHelper::Read(reg, mask)==value)
		break;
	if(u==retry)
		throw DeviceNotReadyException();
	}
}

VOID IoHelper::Wait(RO32& reg, UINT mask, UINT value, UINT timeout)
{
UINT64 time=SystemTimer::GetTickCount64();
UINT64 end=time+timeout;
while(time<end)
	{
	if(IoHelper::Read(reg, mask)==value)
		return;
	Task::Sleep(10);
	time=SystemTimer::GetTickCount64();
	}
throw TimeoutException();
}

}