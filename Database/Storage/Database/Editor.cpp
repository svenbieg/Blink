//============
// Editor.cpp
//============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#include "Editor.h"


//=======
// Using
//=======

#include "Storage/Database/Database.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//==================
// Con-/Destructors
//==================

Editor::~Editor()
{
Cancel();
m_Database->m_Mutex.Unlock();
}


//========
// Common
//========

UINT Editor::Allocate(UINT count)
{
throw NotImplementedException();
return 0;
}

VOID Editor::Free(UINT block, UINT count)
{
throw NotImplementedException();
}

VOID Editor::Save()
{
throw NotImplementedException();
}


//==========================
// Con-/Destructors Private
//==========================

Editor::Editor(Database* database):
m_Database(database)
{
m_Database->m_Mutex.Lock();
}


//================
// Common Private
//================

VOID Editor::Cancel()
{
m_ChangedEntries.clear();
}

}}