//=========
// Entry.h
//=========

// Variable entry with error-correction and skip-bits.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Entry

#pragma once


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Storage/Database/Updates/EntryUpdate.h"
#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//======
// Type
//======

constexpr UINT ENTRY_TYPE(UINT Value)
{
UINT value=TypeHelper::BigEndian(Value);
while((value&0xFF)==0)
	value>>=8;
return value;
}


//======================
// Forward-Declarations
//======================

class Database;
class Editor;


//=======
// Entry
//=======

class Entry: public virtual Object
{
public:
	// Using
	using EntryUpdate=Storage::Database::Updates::EntryUpdate;
	using InputStream=Storage::Streams::InputStream;
	using Mutex=Concurrency::Mutex;
	using OutputStream=Storage::Streams::OutputStream;
	using WriteLock=Concurrency::WriteLock;

	// Friends
	friend Database;
	friend Editor;
	friend EntryUpdate;

	// Con-/Destructors
	~Entry();

protected:
	// Con-/Destructors
	Entry(Database* Database, UINT Id, UINT Type);
	template <class _entry_t> static Handle<_entry_t> Create(Database* Database, UINT Id)
		{
		auto& mutex=GetEntriesMutex(Database);
		WriteLock lock(mutex);
		auto open=GetEntry(Database, Id);
		if(open)
			{
			auto entry=dynamic_cast<_entry_t*>(open);
			if(!entry)
				throw InvalidArgumentException();
			return entry;
			}
		auto entry=Object::Create<_entry_t>(Database, Id);
		SetEntry(Database, Id, entry);
		return entry;
		}

	// Common
	VOID ClearUpdate();
	Handle<Volume> GetVolume()const;
	virtual VOID Invalidate(Editor* Editor);
	virtual SIZE_T ReadEntry(InputStream* Stream);
	UINT Release()noexcept override;
	SIZE_T WriteToBlock(UINT Id);
	virtual SIZE_T WriteEntry(OutputStream* Stream);
	SIZE_T WriteUpdates(OutputStream* Stream);
	Handle<Database> m_Database;
	UINT m_Id;
	Mutex m_Mutex;
	UINT m_Parent;
	UINT m_Size;
	UINT m_Type;
	EntryUpdate* m_Update;

private:
	// Common
	static Mutex& GetEntriesMutex(Database* Database);
	static Entry* GetEntry(Database* Database, UINT Id);
	static VOID SetEntry(Database* Database, UINT Id, Entry* Entry);
};

}}