//==================
// StreamHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Buffer.h"
#include "StreamHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//========
// Common
//========

SIZE_T StreamHelper::Copy(OutputStream* dst, InputStream* src, SIZE_T size)
{
if(!dst||!src)
	return 0;
if(!size)
	size=src->Available();
Handle<Buffer> buf=new Buffer(PAGE_SIZE);
BYTE* buf_ptr=buf->Begin();
SIZE_T pos=0;
while(pos<size)
	{
	SIZE_T copy=TypeHelper::Min(size-pos, PAGE_SIZE);
	SIZE_T read=src->Read(buf_ptr, copy);
	if(!read)
		break;
	dst->Write(buf_ptr, read);
	pos+=read;
	}
return pos;
}

}}