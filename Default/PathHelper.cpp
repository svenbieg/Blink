//================
// PathHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "PathHelper.h"


//===========
// Templates
//===========

template <class _char_t> UINT PathGetComponentLength(_char_t const* path)
{
if(!path)
	return 0;
UINT pos=0;
for(; path[pos]; pos++)
	{
	if(PathHelper::IsSeparator(path[pos]))
		break;
	}
return pos;
}

template <class _char_t> Handle<String> PathGetDirectory(_char_t const* path)
{
UINT len=PathHelper::GetDirectoryLength(path);
if(!len)
	return nullptr;
return String::Create(len, path);
}

template <class _char_t> UINT PathGetDirectoryLength(_char_t const* path)
{
UINT len=StringHelper::Length(path);
if(len==0)
	return 0;
UINT pos=len-1;
for(; pos>0; pos--)
	{
	if(PathHelper::IsSeparator(path[pos]))
		return pos+1;
	}
return 0;
}

template <class _char_t> _char_t const* PathGetExtension(_char_t const* path)
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

template <class _char_t> Handle<String> PathGetHostName(_char_t const* path)
{
UINT pos=0;
if(StringHelper::FindChars(path, ":/", &pos))
	{
	if(path[pos]==':')
		{
		pos+=2;
		UINT len=0;
		StringHelper::FindChar(&path[pos], '/', &len);
		return String::Create(len, &path[pos]);
		}
	return String::Create(pos, path);
	}
return path;
}

template <class _char_t> _char_t const* PathGetLastComponent(_char_t const* path)
{
if(!path)
	return nullptr;
UINT len=PathHelper::GetDirectoryLength(path);
if(path[len]==0)
	return nullptr;
return &path[len];
}

template <class _char_t> BOOL PathIsSeparator(_char_t c)
{
if(CharHelper::Compare(c, '\\')==0)
	return true;
if(CharHelper::Compare(c, '/')==0)
	return true;
return false;
}


//========
// Common
//========

UINT PathHelper::GetComponentLength(LPCSTR path)
{
return PathGetComponentLength(path);
}

UINT PathHelper::GetComponentLength(LPCWSTR path)
{
return PathGetComponentLength(path);
}

Handle<String> PathHelper::GetDirectory(LPCSTR path)
{
return PathGetDirectory(path);
}

Handle<String> PathHelper::GetDirectory(LPCWSTR path)
{
return PathGetDirectory(path);
}

UINT PathHelper::GetDirectoryLength(LPCSTR path)
{
return PathGetDirectoryLength(path);
}

UINT PathHelper::GetDirectoryLength(LPCWSTR path)
{
return PathGetDirectoryLength(path);
}

LPCSTR PathHelper::GetExtension(LPCSTR path)
{
return PathGetExtension(path);
}

LPCWSTR PathHelper::GetExtension(LPCWSTR path)
{
return PathGetExtension(path);
}

Handle<String> PathHelper::GetHostName(LPCSTR path)
{
return PathGetHostName(path);
}

Handle<String> PathHelper::GetHostName(LPCWSTR path)
{
return PathGetHostName(path);
}

Handle<String> PathHelper::GetHostName(Handle<String> const& path)
{
if(!path)
	return nullptr;
return PathGetHostName(path->Begin());
}

LPCSTR PathHelper::GetLastComponent(LPCSTR path)
{
return PathGetLastComponent(path);
}

LPCWSTR PathHelper::GetLastComponent(LPCWSTR path)
{
return PathGetLastComponent(path);
}

BOOL PathHelper::IsSeparator(CHAR c)
{
return PathIsSeparator(c);
}

BOOL PathHelper::IsSeparator(WCHAR c)
{
return PathIsSeparator(c);
}
