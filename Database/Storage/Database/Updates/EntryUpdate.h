//===============
// EntryUpdate.h
//===============

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//======================
// Forward-Declarations
//======================

namespace Storage
{
namespace Database
	{
	class Entry;
	}
}


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {
		namespace Updates {


//==============
// Entry-Update
//==============

class EntryUpdate
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	friend Entry;

	// Con-/Destructors
	virtual ~EntryUpdate()=default;

	// Common
	virtual SIZE_T WriteToStream(OutputStream* Stream)=0;

protected:
	// Con-/Destructors
	EntryUpdate(Entry* Entry): m_Entry(Entry), m_Next(nullptr) {}

	// Common
	static EntryUpdate** AppendUpdate(Entry* Entry);
	static EntryUpdate** GetNext(EntryUpdate* Update);
	static EntryUpdate** GetUpdate(Entry* Entry);
	EntryUpdate* m_Next;
	Entry* m_Entry;
};

}}}