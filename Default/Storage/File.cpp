//==========
// File.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "File.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<String> File::GetName()const
{
return PathGetLastComponent(hPath->Begin());
}

Handle<String> File::GetPath()const
{
return hPath;
}


//============================
// Con-/Destructors Protected
//============================

File::File(Handle<String> path):
hPath(path)
{}

}