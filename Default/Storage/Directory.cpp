//===============
// Directory.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Directory.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<String> Directory::GetName()const
{
return PathGetLastComponent(hPath->Begin());
}

Handle<String> Directory::GetPath()const
{
return hPath;
}


//============================
// Con-/Destructors Protected
//============================

Directory::Directory(Handle<String> path):
hPath(path)
{}

}