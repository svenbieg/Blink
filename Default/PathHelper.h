//==============
// PathHelper.h
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

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
	static UINT GetComponentLength(LPCTSTR Path)noexcept;
	static Handle<String> GetDirectory(LPCTSTR Path);
	static UINT GetDirectoryLength(LPCTSTR Path)noexcept;
	static LPCTSTR GetExtension(LPCTSTR Path)noexcept;
	static inline LPCTSTR GetExtension(Handle<String> const& Path)
		{
		return Path? GetExtension(Path->Begin()): nullptr;
		}
	static Handle<String> GetHostName(LPCTSTR Path);
	static LPCTSTR GetLastComponent(LPCTSTR Path)noexcept;
	static Handle<String> GetName(LPCTSTR Path);
	static Handle<String> GetParentDirectory(LPCTSTR Path);
	static UINT SetExtension(LPTSTR Path, UINT Size, LPCTSTR Extension);
};
