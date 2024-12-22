//=============
// Workspace.h
//=============

#pragma once


//=======
// Using
//=======

#include "Collections/List.h"
#include "Storage/Virtual/Directory.h"
#include "Storage/Directory.h"


//===========
// Namespace
//===========

namespace Storage {


//===========
// Workspace
//===========

class Workspace: public Directory
{
private:
	// Using
	using DirectoryList=Collections::List<Handle<Directory>>;

public:
	// Con-/Destructors
	Workspace(Handle<String> Name);

	// Common
	BOOL Add(Handle<String> Name, Handle<Object> Object);
	VOID AddDirectory(Handle<Storage::Directory> Directory);
	Handle<File> CreateFile(Handle<String> Path, FileCreateMode Create=FileCreateMode::OpenExisting, FileAccessMode Access=FileAccessMode::ReadOnly, FileShareMode Share=FileShareMode::ShareRead)override;
	Handle<DirectoryIterator> First()override;
	Handle<Object> Get(Handle<String> Path)override;

private:
	// Common
	Handle<DirectoryList> m_Directories;
	Handle<Storage::Virtual::Directory> m_Virtual;
};

}