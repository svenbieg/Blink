//==========
// Editor.h
//==========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#pragma once


//=======
// Using
//=======

#include "Collections/index.hpp"
#include "Storage/Database/Entry.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//======================
// Forward-Declarations
//======================

class Database;


//========
// Editor
//========

class Editor: public Object
{
public:
	// Using
	using EntryIndex=Collections::index<Handle<Entry>>;

	// Friends
	friend Database;
	friend Entry;
	friend Object;

	// Con-/Destructors
	~Editor();

	// Common
	UINT Allocate(UINT Count=1);
	VOID Free(UINT Block, UINT Count=1);
	VOID Save();

private:
	// Con-/Destructors
	Editor(Database* Database);
	static inline Handle<Editor> Create(Database* Database) { return Object::Create<Editor>(Database); }

	// Common
	VOID Cancel();
	EntryIndex m_ChangedEntries;
	Database* m_Database;
};

}}