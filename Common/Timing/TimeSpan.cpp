//==============
// TimeSpan.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "TimeSpan.h"


//===========
// Namespace
//===========

namespace Timing {


//========
// Access
//========

Handle<String> TimeSpan::ToString()
{
SharedLock lock(m_Mutex);
return ToString(m_Value);
}

Handle<String> TimeSpan::ToString(UINT us)
{
CHAR pstr[16];
ToString(us, pstr, 16);
return pstr;
}

UINT TimeSpan::ToString(UINT us, LPSTR str, UINT size)
{
UINT days=us/24/60/60;
us-=days*24*60*60;
UINT hours=us/60/60;
us-=hours*60*60;
UINT mins=us/60;
us-=mins*60;
if(days>0)
	return StringHelper::Print(str, size, "%ud %uh", days, hours);
if(hours>0)
	return StringHelper::Print(str, size, "%u:%02uh", hours, mins);
if(mins>0)
	return StringHelper::Print(str, size, "%um", mins);
return StringHelper::Print(str, size, "%us", us);
}

}