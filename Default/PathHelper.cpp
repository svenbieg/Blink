//================
// PathHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "CharHelper.h"
#include "PathHelper.h"


//========
// Access
//========

template <class _str_t> inline UINT PathGetComponentLength(_str_t path)
{
if(!path)
	return 0;
UINT pos=0;
for(; path[pos]; pos++)
	{
	if(PathIsSeparator(path[pos]))
		break;
	}
return pos;
}

UINT PathGetComponentLength(LPCSTR path)
{
return PathGetComponentLength<LPCSTR>(path);
}

UINT PathGetComponentLength(LPCWSTR path)
{
return PathGetComponentLength<LPCWSTR>(path);
}

template <class _str_t> inline UINT PathGetDirectoryLength(_str_t path)
{
UINT len=StringLength(path);
if(len==0)
	return 0;
UINT pos=len-1;
for(; pos>0; pos--)
	{
	if(PathIsSeparator(path[pos]))
		return pos+1;
	}
return 0;
}

template <class _str_t> inline Handle<String> PathGetDirectory(_str_t path)
{
UINT len=PathGetDirectoryLength<_str_t>(path);
if(!len)
	return nullptr;
return new String(len, path);
}

Handle<String> PathGetDirectory(LPCSTR path)
{
return PathGetDirectory<LPCSTR>(path);
}

Handle<String> PathGetDirectory(LPCWSTR path)
{
return PathGetDirectory<LPCWSTR>(path);
}

UINT PathGetDirectoryLength(LPCSTR path)
{
return PathGetDirectoryLength<LPCSTR>(path);
}

UINT PathGetDirectoryLength(LPCWSTR path)
{
return PathGetDirectoryLength<LPCWSTR>(path);
}

template <class _str_t> inline _str_t PathGetExtension(_str_t path)
{
if(!path)
	return nullptr;
UINT pos=0;
for(; path[pos]; pos++)
	{
	if(path[pos]=='.')
		break;
	}
if(path[pos]!='.')
	return nullptr;
pos++;
if(!path[pos])
	return nullptr;
return &path[pos];
}

LPCSTR PathGetExtension(LPCSTR path)
{
return PathGetExtension<LPCSTR>(path);
}

LPCWSTR PathGetExtension(LPCWSTR path)
{
return PathGetExtension<LPCWSTR>(path);
}

template <class _str_t> inline _str_t PathGetLastComponent(_str_t path)
{
if(!path)
	return nullptr;
UINT len=PathGetDirectoryLength<_str_t>(path);
if(path[len]==0)
	return nullptr;
return &path[len];
}

LPCSTR PathGetLastComponent(LPCSTR path)
{
return PathGetLastComponent<LPCSTR>(path);
}

LPCWSTR PathGetLastComponent(LPCWSTR path)
{
return PathGetLastComponent<LPCWSTR>(path);
}

template <class _char_t> inline BOOL PathIsSeparator(_char_t c)
{
if(CharCompare(c, '\\')==0)
	return true;
if(CharCompare(c, '/')==0)
	return true;
return false;
}

BOOL PathIsSeparator(CHAR c)
{
return PathIsSeparator<CHAR>(c);
}

BOOL PathIsSeparator(WCHAR c)
{
return PathIsSeparator<WCHAR>(c);
}
