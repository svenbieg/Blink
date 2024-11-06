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
return PathGetLastComponent(m_Path->Begin());
}

Handle<String> Directory::GetPath()const
{
return m_Path;
}


//============================
// Con-/Destructors Protected
//============================

Directory::Directory(Handle<String> path):
m_Path(path)
{}

}