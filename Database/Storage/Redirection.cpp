//=================
// Redirection.cpp
//=================

// Bad blocks are redirected on backup-failure.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Redirection

#include "Redirection.h"


//===========
// Namespace
//===========

namespace Storage {


//==========================
// Con-/Destructors Private
//==========================

Redirection::Redirection(Volume* volume, FileCreateMode create):
m_Size(0),
m_Volume(volume)
{
switch(create)
	{
	case FileCreateMode::OpenExisting:
		{
		auto page=volume->ReadPage(0, 0);
		m_Size=ReadFromStream(page);
		break;
		}
	case FileCreateMode::CreateAlways:
		{
		volume->Erase(0);
		break;
		}
	default:
		throw InvalidArgumentException();
	}
}


//==================
// Common Protected
//==================

WORD Redirection::ReadFromStream(InputStream* stream)
{
WORD size=0;
while(stream->Available())
	{
	UINT id=0;
	stream->Read(&id, sizeof(UINT));
	if(id==-1)
		break;
	size+=sizeof(UINT);
	if(id!='RIDR')
		continue;
	UINT block[2];
	size+=stream->Read(&block[0], sizeof(UINT));
	size+=stream->Read(&block[1], sizeof(UINT));
	if(block[0]!=~block[1])
		continue;
	if(m_Blocks.remove(block[0]))
		continue;
	m_Blocks.add(block[0]);
	}
return size;
}

}