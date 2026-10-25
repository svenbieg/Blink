//============
// Database.h
//============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Concurrency/WriteLock.h"
#include "Storage/Database/Editor.h"
#include "Storage/Database/Map.h"
#include "Storage/Database/Node.h"
#include "Storage/Encoding/Unsigned.h"
#include "Storage/File.h"
#include "Storage/Redirection.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//==========
// Database
//==========

class Database: public Object
{
public:
	// Using
	using Unsigned=Storage::Encoding::Unsigned;
	using BlockMap=Map<Unsigned, Unsigned>;
	using EntryMap=Collections::map<UINT, Entry*>;
	using Mutex=Concurrency::Mutex;
	using WriteLock=Concurrency::WriteLock;

	// Friends
	friend Editor;
	friend Entry;
	friend Object;

	// Con-/Destructors
	static inline Handle<Database> Create(Volume* Volume, FileCreateMode Create=FileCreateMode::OpenExisting) { return Object::Create<Database>(Volume, Create); }

	// Common
	inline Handle<Editor> Edit() { return Editor::Create(this); }

private:
	// Entries
	static const UINT ID_HEADER_0=Redirection::REDIR_SIZE;
	static const UINT ID_HEADER_1=ID_HEADER_0+1;

	// Settings
	static const UINT HEADER_SIZE=2;

	// Con-/Destructors
	Database(Volume* Volume, FileCreateMode Create);

	// Common
	VOID Initialize();
	Handle<Node> ReadHeader();
	VOID ValidateHeader(Node* Header);
	EntryMap m_Entries;
	Mutex m_EntriesMutex;
	Handle<Node> m_Header;
	Handle<BlockMap> m_MapFree;
	Mutex m_Mutex;
	Handle<Redirection> m_Redirection;
	UINT m_Used;
	Handle<Volume> m_Volume;
};

}}