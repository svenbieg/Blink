//===========
// Entry.cpp
//===========

// Variable entry with error-correction and skip-bits.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Entry

#include "Entry.h"


//=======
// Using
//=======

#include "Storage/Database/Database.h"
#include "Storage/Database/Editor.h"
#include "Storage/Encoding/Dwarf.h"

using namespace Concurrency;
using namespace Storage::Encoding;


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//==================
// Con-/Destructors
//==================

Entry::~Entry()
{
ClearUpdate();
}


//============================
// Con-/Destructors Protected
//============================

Entry::Entry(Database* database, UINT id, UINT type):
m_Database(database),
m_Id(id),
m_Size(0),
m_Type(type),
m_Update(nullptr)
{}


//==================
// Common Protected
//==================

Handle<Volume> Entry::GetVolume()const
{
return m_Database->m_Volume;
}

VOID Entry::Invalidate(Editor* editor)
{
if(m_Id!=-1)
	editor->m_ChangedEntries.add(this);
}

SIZE_T Entry::ReadEntry(InputStream* stream)
{
SIZE_T size=0;
UINT type=0;
size+=stream->Read(&type, sizeof(UINT));
if(m_Type!=type)
	throw NotFoundException();
size+=Dwarf::Read(stream, &m_Parent);
return size;
}

UINT Entry::Release()noexcept
{
WriteLock lock(m_Database->m_EntriesMutex);
UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
if(ref_count>0)
	return ref_count;
if(m_Id!=-1)
	m_Database->m_Entries.remove(m_Id);
delete this;
return 0;
}

SIZE_T Entry::WriteEntry(OutputStream* stream)
{
SIZE_T size=0;
size+=stream->Write(&m_Type, sizeof(UINT));
size+=Dwarf::Write(stream, m_Parent);
return size;
}

SIZE_T Entry::WriteToBlock(UINT id)
{
auto volume=m_Database->m_Volume;
auto block=Block::Create(volume, id);
WriteEntry(block);
block->Flush();
return block->GetPosition();
}

SIZE_T Entry::WriteUpdates(OutputStream* stream)
{
SIZE_T size=0;
auto update=m_Update;
while(update)
	{
	size+=update->WriteToStream(stream);
	update=update->m_Next;
	}
return size;
}


//================
// Common Private
//================

VOID Entry::ClearUpdate()
{
while(m_Update)
	{
	auto update=m_Update;
	m_Update=update->m_Next;
	delete update;
	}
}

Mutex& Entry::GetEntriesMutex(Database* database)
{
return database->m_EntriesMutex;
}

Entry* Entry::GetEntry(Database* database, UINT id)
{
Entry* entry=nullptr;
database->m_Entries.try_get(id, &entry);
return entry;
}

VOID Entry::SetEntry(Database* database, UINT id, Entry* entry)
{
database->m_Entries.set(id, entry);
}

}}