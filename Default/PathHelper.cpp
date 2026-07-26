//================
// PathHelper.cpp
//================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#include "PathHelper.h"


//=============
// Path-Helper
//=============

UINT PathHelper::GetComponentLength(LPCTSTR path)noexcept
{
if(!path)
	return 0;
UINT pos=0;
for(; path[pos]; pos++)
	{
	if(CharHelper::Equal(path[pos], "\\/"))
		break;
	}
return pos;
}

Handle<String> PathHelper::GetDirectory(LPCTSTR path)
{
UINT len=PathHelper::GetDirectoryLength(path);
if(!len)
	return nullptr;
return String::Create(len, path);
}

UINT PathHelper::GetDirectoryLength(LPCTSTR path)noexcept
{
UINT len=StringHelper::Length(path);
if(len==0)
	return 0;
UINT pos=len-1;
for(; pos>0; pos--)
	{
	if(CharHelper::Equal(path[pos], "\\/"))
		return pos+1;
	}
return 0;
}

LPCTSTR PathHelper::GetExtension(LPCTSTR path)noexcept
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

Handle<String> PathHelper::GetHostName(LPCTSTR path)
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

LPCTSTR PathHelper::GetLastComponent(LPCTSTR path)noexcept
{
if(!path)
	return nullptr;
UINT len=PathHelper::GetDirectoryLength(path);
if(path[len]==0)
	return nullptr;
return &path[len];
}

Handle<String> PathHelper::GetName(LPCTSTR path)
{
if(!path||!path[0])
	return nullptr;
UINT len=StringHelper::Length(path);
UINT end=len;
for(UINT pos=len-1; pos>0; pos--)
	{
	if(CharHelper::Equal(path[pos], '.'))
		{
		end=pos;
		continue;
		}
	if(CharHelper::Equal(path[pos], "\\/"))
		{
		if(pos+1==end)
			return nullptr;
		pos++;
		return String::Create(end-pos, &path[pos]);
		}
	}
return String::Create(end, path);
}

Handle<String> PathHelper::GetParentDirectory(LPCTSTR path)
{
UINT len=StringHelper::Length(path);
if(!len)
	return nullptr;
UINT pos=len-1;
if(CharHelper::Equal(path[pos], "\\/"))
	pos--;
for(; pos>0; pos--)
	{
	if(CharHelper::Equal(path[pos], "\\/"))
		break;
	}
if(pos+1==len)
	return nullptr;
return String::Create(pos, path);
}

UINT PathHelper::SetExtension(LPTSTR path, UINT size, LPCTSTR ext)
{
UINT ext_len=0;
if(ext)
	{
	if(CharHelper::Equal(ext[0], '.'))
		ext++;
	ext_len=StringHelper::Length(ext);
	if(ext_len==0)
		ext=nullptr;
	}
UINT pos=0;
if(StringHelper::FindChar(path, '.', &pos))
	{
	if(!ext)
		{
		path[pos]=0;
		return pos;
		}
	}
if(pos+ext_len+2>=size)
	throw BufferOverrunException();
path[pos++]='.';
pos+=StringHelper::Copy(&path[pos], size-pos, ext);
return pos;
}
