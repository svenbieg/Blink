//==============
// Database.cpp
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#include "Database.h"


//=======
// Using
//=======

#include "Storage/Database/Editor.h"
#include "Storage/Database/Map.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//==========================
// Con-/Destructors Private
//==========================

Database::Database(Volume* volume, FileCreateMode create):
m_Used(0),
m_Volume(volume)
{
auto header=ReadHeader();
switch(create)
	{
	case FileCreateMode::CreateNew:
		{
		if(header)
			throw AlreadyExistsException();
		break;
		}
	case FileCreateMode::OpenExisting:
		{
		if(!header)
			throw NotFoundException();
		break;
		}
	}
switch(create)
	{
	case FileCreateMode::CreateNew:
	case FileCreateMode::CreateAlways:
		{
		Initialize();
		return;
		}
	case FileCreateMode::OpenAlways:
		{
		if(!header)
			{
			Initialize();
			return;
			}
		ValidateHeader(header);
		break;
		}
	case FileCreateMode::OpenExisting:
		{
		ValidateHeader(header);
		break;
		}
	}
m_Redirection=Redirection::Create(m_Volume);
}


//================
// Common Private
//================

VOID Database::Initialize()
{
m_Redirection=Redirection::Create(m_Volume, FileCreateMode::CreateAlways);
m_Used+=Redirection::REDIR_SIZE;
m_Header=Node::Create(this, "Header");
m_Header->SetAttribute("Update", "0");
m_Header->WriteToBlock(ID_HEADER_0);
m_Header->WriteToBlock(ID_HEADER_1);
m_Used+=HEADER_SIZE;
}

Handle<Node> Database::ReadHeader()
{
Handle<Node> header;
for(UINT id=0; id<2; id++)
	{
	try
		{
		header=Node::Create(this, ID_HEADER_0+id);
		}
	catch(InvalidArgumentException)
		{
		continue;
		}
	}
return header;
}

VOID Database::ValidateHeader(Node* header)
{
if(header->m_Id==ID_HEADER_0)
	{
	try
		{
		auto header1=Node::Create(this, ID_HEADER_1);
		if(header1->m_Size!=header->m_Size)
			throw InvalidArgumentException();
		}
	catch(InvalidArgumentException)
		{
		header->WriteToBlock(ID_HEADER_1);
		header->WriteToBlock(ID_HEADER_0);
		}
	}
else
	{
	header->WriteToBlock(ID_HEADER_0);
	header->WriteToBlock(ID_HEADER_1);
	}
m_Header=header;
}

}}