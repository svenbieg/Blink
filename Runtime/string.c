//==========
// string.c
//==========

#include <string.h>


//========
// Common
//========

int memcmp(void const* buf1_ptr, void const* buf2_ptr, size_t size)
{
char const* buf1=(char const*)buf1_ptr;
char const* buf2=(char const*)buf2_ptr;
for(size_t u=0; u<size; u++)
	{
	if(buf1[u]>buf2[u])
		return 1;
	if(buf1[u]<buf2[u])
		return -1;
	}
return 0;
}

void* memcpy(void* dst_ptr, void const* src_ptr, size_t size)
{
char* dst=(char*)dst_ptr;
char const* src=(char const*)src_ptr;
char* end=dst+size;
while(dst<end)
	*dst++=*src++;
return dst;
}

void* memmove(void* dst_ptr, void const* src_ptr, size_t size)
{
char* dst=(char*)dst_ptr;
char const* src=(char const*)src_ptr;
if(dst==src)
	return dst+size;
if(dst>src)
	{
	char* end=(char*)dst_ptr;
	dst+=size;
	src+=size;
	while(dst>=end)
		*--dst=*--src;
	}
else
	{
	char* end=dst+size;
	while(dst<end)
		*dst++=*src++;
	}
return dst+size;
}

void* memset(void* dst_ptr, int value, size_t size)
{
char* dst=(char*)dst_ptr;
char* end=dst+size;
while(dst<end)
	*dst++=(char)value;
return dst;
}
