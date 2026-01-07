//=========
// Queue.h
//=========

#pragma once


//=======
// Using
//=======

#include "Handle.h"


//===========
// Namespace
//===========

namespace Collections {


//=======
// Queue
//=======

template <class _item_t> class Queue
{
public:
	// Con-/Destructors
	Queue()=default;

	// Common
	VOID Append(_item_t* item)
		{
		if(m_Last)
			{
			m_Last->Next=item;
			}
		else
			{
			m_First=item;
			m_Last=item;
			}
		}
	Handle<_item_t> Dequeue()
		{
		if(!m_First)
			return nullptr;
		auto item=m_First;
		if(item->Next)
			{
			m_First=item->Next;
			item->Next=nullptr;
			}
		else
			{
			m_First=nullptr;
			m_Last=nullptr;
			}
		return item;
		}

private:
	// Common
	Handle<_item_t> m_First;
	_item_t* m_Last=nullptr;
};

}