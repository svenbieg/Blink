//==========
// File.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "File.h"
#include "PathHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<String> File::GetName()const
{
return PathHelper::GetLastComponent(m_Path->Begin());
}

Handle<String> File::GetPath()const
{
return m_Path;
}


//============================
// Con-/Destructors Protected
//============================

File::File(Handle<String> path):
m_Path(path)
{}

}