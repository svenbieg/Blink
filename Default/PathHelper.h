//==============
// PathHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//=============
// Path-Helper
//=============

class PathHelper
{
public:
	// Common
	template <class _str_t> static UINT GetComponentLength(_str_t path)
		{
		if(!path)
			return 0;
		UINT pos=0;
		for(; path[pos]; pos++)
			{
			if(IsSeparator(path[pos]))
				break;
			}
		return pos;
		}
	template <class _str_t> static Handle<String> GetDirectory(_str_t path)
		{
		UINT len=GetDirectoryLength(path);
		if(!len)
			return nullptr;
		return new String(len, path);
		}
	template <class _str_t> static UINT GetDirectoryLength(_str_t path)
		{
		UINT len=StringHelper::Length(path);
		if(len==0)
			return 0;
		UINT pos=len-1;
		for(; pos>0; pos--)
			{
			if(IsSeparator(path[pos]))
				return pos+1;
			}
		return 0;
		}
	template <class _str_t> static _str_t GetExtension(_str_t path)
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
	template <class _str_t> static _str_t GetLastComponent(_str_t path)
		{
		if(!path)
			return nullptr;
		UINT len=GetDirectoryLength(path);
		if(path[len]==0)
			return nullptr;
		return &path[len];
		}
	template <class _char_t> static BOOL IsSeparator(_char_t c)
		{
		if(CharHelper::Compare(c, '\\')==0)
			return true;
		if(CharHelper::Compare(c, '/')==0)
			return true;
		return false;
		}
};
