//=================
// EntryUpdate.cpp
//=================

#include "EntryUpdate.h"


//=======
// Using
//=======

#include "Storage/Database/Entry.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {
		namespace Updates {


//==================
// Common Protected
//==================

EntryUpdate** EntryUpdate::AppendUpdate(Entry* entry)
{
auto update_ptr=&entry->m_Update;
while(*update_ptr)
	{
	auto update=*update_ptr;
	auto update_ptr=&update->m_Next;
	}
return update_ptr;
}

EntryUpdate** EntryUpdate::GetNext(EntryUpdate* update)
{
return &update->m_Next;
}

EntryUpdate** EntryUpdate::GetUpdate(Entry* entry)
{
return &entry->m_Update;
}

}}}