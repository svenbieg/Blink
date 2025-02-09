//===============
// Directory.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Directory.h"
#include "PathHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<String> Directory::GetName()const
{
return PathHelper::GetLastComponent(m_Path->Begin());
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