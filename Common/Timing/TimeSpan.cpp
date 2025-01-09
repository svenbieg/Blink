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

UINT TimeSpan::Get()
{
UINT value=m_Value;
Reading(this, value);
return value;
}

Handle<String> TimeSpan::ToString(UINT sec)
{
CHAR str[16];
ToString(str, 16, sec);
return str;
}

UINT TimeSpan::ToString(LPSTR str, UINT size, UINT sec)
{
UINT days=sec/24/60/60;
sec-=days*24*60*60;
UINT hours=sec/60/60;
sec-=hours*60*60;
UINT mins=sec/60;
sec-=mins*60;
if(days>0)
	return StringHelper::Print(str, size, "%ud %uh", days, hours);
if(hours>0)
	return StringHelper::Print(str, size, "%u:%02uh", hours, mins);
if(mins>0)
	return StringHelper::Print(str, size, "%um", mins);
return StringHelper::Print(str, size, "%us", sec);
}

SIZE_T TimeSpan::WriteToStream(OutputStream* stream)
{
if(!stream)
	return sizeof(UINT);
UINT value=m_Value;
Reading(this, value);
return stream->Write(&value, sizeof(UINT));
}


//==============
// Modification
//==============

SIZE_T TimeSpan::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(UINT);
UINT value;
SIZE_T size=stream->Read(&value, sizeof(UINT));
if(size==sizeof(UINT))
	Set(value, notify);
return size;
}

BOOL TimeSpan::Set(UINT value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}

}