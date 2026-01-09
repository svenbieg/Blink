//===============
// ForwardList.h
//===============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"
#include <cstddef>


//========
// Macros
//========

#define FWD_LIST(item_t, member) item_t, offsetof(item_t, member)


//===========
// Namespace
//===========

namespace Collections {


//======
// Link
//======

template <class _item_t> struct FwdLink
{
_item_t* Next=nullptr;
};


//==============
// Forward-List
//==============

template <class _item_t, size_t _member> class ForwardList
{
public:
	// Comparison
	typedef bool (*COMPARE_FN)(_item_t* First, _item_t* Second);

	// Con-/Destructors
	ForwardList()=default;
	ForwardList(ForwardList const&)=delete;

	// Access
	inline operator bool()const noexcept { return m_First!=nullptr; }
	inline UINT Count(UINT Max=0)const { return Count(m_First, Max); }
	static UINT Count(_item_t const* First, UINT Max=0)
		{
		UINT count=0;
		auto current=First;
		while(current)
			{
			if(++count==Max)
				break;
			current=*MemberNext(current);
			}
		return count;
		}
	inline _item_t* First()const noexcept { return m_First; }
	static inline _item_t* Next(_item_t* Item)noexcept
		{
		if(!Item)
			return nullptr;
		return *MemberNext(Item);
		}

	// Modification
	inline VOID Append(_item_t* Item) { Append(&m_First, Item); }
	static VOID Append(_item_t** First, _item_t* Item)
		{
		assert(First!=nullptr);
		assert(Item!=nullptr);
		auto current_ptr=First;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			assert(current!=Item);
			auto current_next=MemberNext(current);
			current_ptr=current_next;
			}
		*current_ptr=Item;
		}
	inline VOID Insert(_item_t* Item, COMPARE_FN Compare) { Insert(&m_First, Item, Compare); }
	static VOID Insert(_item_t** First, _item_t* Item, COMPARE_FN Compare)
		{
		assert(First!=nullptr);
		assert(Item!=nullptr);
		auto current_ptr=First;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			assert(current!=Item);
			if(Compare(Item, current))
				{
				auto item_next=MemberNext(Item);
				*item_next=current;
				*current_ptr=Item;
				return;
				}
			current_ptr=MemberNext(current);
			}
		*current_ptr=Item;
		}
	inline _item_t* Remove(_item_t* Item) { return Remove(&m_First, Item); }
	static _item_t* Remove(_item_t** First, _item_t* Item)
		{
		assert(First!=nullptr);
		assert(Item!=nullptr);
		auto current_ptr=First;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			if(current==Item)
				{
				auto current_next=MemberNext(current);
				*current_ptr=*current_next;
				*current_next=nullptr;
				return *current_ptr;
				}
			current_ptr=MemberNext(current);
			}
		return nullptr;
		}
	inline _item_t* RemoveFirst() { return RemoveFirst(&m_First); }
	static _item_t* RemoveFirst(_item_t** First)
		{
		assert(First!=nullptr);
		auto first=*First;
		if(!first)
			return nullptr;
		auto first_next=MemberNext(first);
		*First=*first_next;
		*first_next=nullptr;
		return first;
		}
	inline BOOL TryRemove(_item_t* Item) { return TryRemove(&m_First, Item); }
	static BOOL TryRemove(_item_t** First, _item_t* Item)
		{
		assert(First!=nullptr);
		assert(Item!=nullptr);
		auto current_ptr=First;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			if(current==Item)
				{
				auto current_next=MemberNext(current);
				*current_ptr=*current_next;
				*current_next=nullptr;
				return true;
				}
			current_ptr=MemberNext(current);
			}
		return false;
		}

private:
	// Common
	static inline _item_t** MemberNext(_item_t const* Item)noexcept
		{
		auto link=(FwdLink<_item_t>*)((SIZE_T)Item+_member);
		return &link->Next;
		}
	_item_t* m_First=nullptr;
};

}