//===============
// Workspace.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Workspace.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

BOOL Workspace::Add(Handle<String> name, Handle<Object> obj)
{
return m_Virtual->Add(name, obj);
}

VOID Workspace::AddDirectory(Handle<Storage::Directory> dir)
{
if(!dir)
	return;
m_Directories->Append(dir);
m_Virtual->Add(dir->GetName(), dir);
}

Handle<DirectoryIterator> Workspace::Begin()
{
return m_Virtual->Begin();
}

Handle<File> Workspace::CreateFile(Handle<String> path, FileCreateMode create, FileAccessMode access, FileShareMode share)
{
if(!path||path->IsEmpty())
	return nullptr;
auto str=path->Begin();
UINT pos=0;
while(PathHelper::IsSeparator(str[pos]))
	pos++;
UINT len=PathHelper::GetComponentLength(&str[pos]);
if(!len)
	return nullptr;
auto name=String::Create(len, &str[pos]);
pos+=len;
if(str[pos]==0)
	return nullptr;
auto obj=m_Virtual->GetChild(name);
if(!obj)
	return nullptr;
auto dir=obj.As<Storage::Directory>();
if(!dir)
	return nullptr;
return dir->CreateFile(&str[pos], create, access, share);
}

Handle<Object> Workspace::Get(Handle<String> path)
{
for(auto it=m_Directories->Begin(); it->HasCurrent(); it->MoveNext())
	{
	auto dir=it->GetCurrent();
	auto obj=dir->Get(path);
	if(obj)
		return obj;
	}
return nullptr;
}


//==========================
// Con-/Destructors Private
//==========================

Workspace::Workspace(Handle<String> name):
Directory(name)
{
m_Directories=DirectoryList::Create();
m_Virtual=Storage::Virtual::Directory::Create("Virtual");
AddDirectory(m_Virtual);
}

}