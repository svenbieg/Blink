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
	namespace Virtual {


//==================
// Con-/Destructors
//==================

Directory::Directory(Handle<String> path):
Storage::Directory(path)
{}


//========
// Common
//========

BOOL Directory::Add(Handle<String> name, Handle<Object> obj, BOOL notify)
{
if(!obj)
	return false;
if(!m_Map.add(name, obj))
	return false;
if(notify)
	Changed(this);
return true;
}

VOID Directory::Clear()
{
m_Map.clear();
}

Handle<Object> Directory::GetChild(Handle<String> name)
{
if(!name||name->IsEmpty())
	return nullptr;
return m_Map.get(name);
}

BOOL Directory::Remove(Handle<String> name, BOOL notify)
{
if(!m_Map.remove(name))
	return false;
if(notify)
	Changed(this);
return true;
}

VOID Directory::RemoveAll()
{
if(m_Map.clear())
	Changed(this);
}


//===================
// Storage.Directory
//===================

Handle<Storage::File> Directory::CreateFile(Handle<String> path, FileCreateMode create, FileAccessMode access, FileShareMode share)
{
return nullptr;
}

Handle<Storage::DirectoryIterator> Directory::First()
{
return new DirectoryIterator(this);
}

Handle<Object> Directory::Get(Handle<String> path)
{
if(!path||path->IsEmpty())
	return nullptr;
auto path_ptr=path->Begin();
UINT pos=0;
while(PathHelper::IsSeparator(path_ptr[pos]))
	pos++;
UINT uclen=PathHelper::GetComponentLength(&path_ptr[pos]);
if(!uclen)
	return this;
Handle<String> name=new String(uclen, &path_ptr[pos]);
pos+=uclen;
auto obj=m_Map.get(name);
if(!obj)
	return nullptr;
if(path_ptr[pos]==0)
	return obj;
auto sub=obj.As<Storage::Directory>();
if(sub)
	return sub->Get(&path_ptr[pos]);
return nullptr;
}


//===========================
// Iterator Con-/Destructors
//===========================

DirectoryIterator::DirectoryIterator(Handle<Directory> dir):
m_Directory(dir),
m_It(&dir->m_Map)
{
First();
}

}}