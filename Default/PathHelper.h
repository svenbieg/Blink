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
	static UINT GetComponentLength(LPCSTR Path)noexcept;
	static UINT GetComponentLength(LPCWSTR Path)noexcept;
	static Handle<String> GetDirectory(LPCSTR Path)noexcept;
	static Handle<String> GetDirectory(LPCWSTR Path)noexcept;
	static UINT GetDirectoryLength(LPCSTR Path)noexcept;
	static UINT GetDirectoryLength(LPCWSTR Path)noexcept;
	static LPCSTR GetExtension(LPCSTR Path)noexcept;
	static LPCWSTR GetExtension(LPCWSTR Path)noexcept;
	static Handle<String> GetHostName(LPCSTR Path)noexcept;
	static Handle<String> GetHostName(LPCWSTR Path)noexcept;
	static Handle<String> GetHostName(Handle<String> Path)noexcept;
	static LPCSTR GetLastComponent(LPCSTR Path)noexcept;
	static LPCWSTR GetLastComponent(LPCWSTR Path)noexcept;
	static LPCTSTR GetLastComponent(Handle<String> Path)noexcept;
	static Handle<String> GetName(LPCSTR Path)noexcept;
	static Handle<String> GetName(LPCWSTR Path)noexcept;
	static Handle<String> GetName(Handle<String> Path)noexcept;
	static UINT SetExtension(LPSTR Path, UINT Size, LPCSTR Extension);
	static UINT SetExtension(LPWSTR Path, UINT Size, LPCWSTR Extension);
};
