//=============
// Directory.h
//=============

#pragma once


//=======
// Using
//=======

#include "Collections/shared_map.hpp"
#include "Storage/Directory.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Virtual {


//======================
// Forward-Declarations
//======================

class DirectoryIterator;


//===========
// Directory
//===========

class Directory: public Storage::Directory
{
public:
	// Friends
	friend DirectoryIterator;

	// Con-/Destructors
	static inline Handle<Directory> Create(Handle<String> Path=nullptr) { return new Directory(Path); }

	// Common
	BOOL Add(Handle<String> Name, Handle<Object> Object, BOOL Notify=true);
	Event<Directory> Changed;
	VOID Clear();
	Handle<Object> GetChild(Handle<String> Name);
	BOOL Remove(Handle<String> Name, BOOL Notify=true);
	VOID RemoveAll();

	// Storage.Directory
	Handle<Storage::DirectoryIterator> Begin()override;
	Handle<Storage::File> CreateFile(Handle<String> Path, FileCreateMode Create=FileCreateMode::OpenExisting, FileAccessMode Access=FileAccessMode::ReadWrite, FileShareMode Share=FileShareMode::ShareRead)override;
	Handle<Object> Get(Handle<String> Path)override;

private:
	// Con-/Destructors
	Directory(Handle<String> Path);

	// Common
	Collections::shared_map<Handle<String>, Handle<Object>> m_Map;
};


//==========
// Iterator
//==========

class DirectoryIterator: public Storage::DirectoryIterator
{
public:
	// Friends
	friend Directory;

	// Common
	BOOL Find(Handle<String> Name) { return m_It.find(Name); }
	BOOL First() { return m_It.set_position(0); }
	Handle<Object> GetCurrent()const override { return m_It->get_value(); }
	UINT GetPosition()const { return m_It.get_position(); }
	BOOL HasCurrent()const override { return m_It.has_current(); }
	BOOL MoveNext()override { return m_It.move_next(); }
	BOOL MovePrevious() { return m_It.move_previous(); }
	BOOL RemoveCurrent() { return m_It.remove_current(); }
	VOID SetPosition(UINT Position) { m_It.set_position(Position); }

private:
	// Con-/Destructors
	DirectoryIterator(Handle<Directory> Directory);

	// Common
	Handle<Directory> m_Directory;
	typename Collections::shared_map<Handle<String>, Handle<Object>>::iterator m_It;
};

}}