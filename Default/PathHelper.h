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
	static UINT GetComponentLength(LPCSTR Path);
	static UINT GetComponentLength(LPCWSTR Path);
	static Handle<String> GetDirectory(LPCSTR Path);
	static Handle<String> GetDirectory(LPCWSTR Path);
	static UINT GetDirectoryLength(LPCSTR Path);
	static UINT GetDirectoryLength(LPCWSTR Path);
	static LPCSTR GetExtension(LPCSTR Path);
	static LPCWSTR GetExtension(LPCWSTR Path);
	static Handle<String> GetHostName(LPCSTR Path);
	static Handle<String> GetHostName(LPCWSTR Path);
	static Handle<String> GetHostName(Handle<String> const& Path);
	static LPCSTR GetLastComponent(LPCSTR Path);
	static LPCWSTR GetLastComponent(LPCWSTR Path);
	static BOOL IsSeparator(CHAR Char);
	static BOOL IsSeparator(WCHAR Char);
};
