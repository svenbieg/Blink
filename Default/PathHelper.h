//==============
// PathHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//========
// Access
//========

Handle<String> PathGetDirectory(LPCSTR Path);
Handle<String> PathGetDirectory(LPCWSTR Path);
UINT PathGetComponentLength(LPCSTR Path);
UINT PathGetComponentLength(LPCWSTR Path);
UINT PathGetDirectoryLength(LPCSTR Path);
UINT PathGetDirectoryLength(LPCWSTR Path);
LPCSTR PathGetExtension(LPCSTR Path);
LPCWSTR PathGetExtension(LPCWSTR Path);
LPCSTR PathGetLastComponent(LPCSTR Path);
LPCWSTR PathGetLastComponent(LPCWSTR Path);
BOOL PathIsSeparator(CHAR Char);
BOOL PathIsSeparator(WCHAR Char);
