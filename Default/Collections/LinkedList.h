//==============
// LinkedList.h
//==============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"
#include <cstddef>


//========
// Macros
//========

#define LNK_LIST(item_t, member) item_t, offsetof(item_t, member)


//===========
// Namespace
//===========

namespace Collections {


//======
// Link
//======

template <class _item_t> struct Link
{
_item_t* Previous=nullptr;
_item_t* Next=nullptr;
};


//=============
// Linked-List
//=============

template <class _item_t, size_t _member> class LinkedList
{
public:
	// Comparison
	typedef bool (*COMPARE_FN)(_item_t* First, _item_t* Second);

	// Con-/Destructors
	LinkedList()=default;
	LinkedList(LinkedList const&)=delete;

	// Access
	inline operator bool()const noexcept { return m_First!=nullptr; }
	inline UINT Count(UINT Max=0)const noexcept { return Count(m_First, Max); }
	static UINT Count(_item_t const* First, UINT Max=0)noexcept
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
	inline _item_t* Last()const noexcept { return m_Last; }
	static _item_t* Next(_item_t* Item)noexcept
		{
		if(!Item)
			return nullptr;
		return *MemberNext(Item);
		}
	static _item_t* Previous(_item_t* Item)noexcept
		{
		if(!Item)
			return nullptr;
		return *MemberPrevious(Item);
		}

	// Modification
	inline VOID Append(_item_t* Item) { Append(&m_First, &m_Last, Item); }
	static VOID Append(_item_t** First, _item_t** Last, _item_t* Item)
		{
		assert(First!=nullptr);
		assert(Last!=nullptr);
		assert(Item!=nullptr);
		auto last=*Last;
		if(!last)
			{
			*First=Item;
			*Last=Item;
			return;
			}
		auto next_ptr=MemberNext(last);
		*next_ptr=Item;
		auto prev_ptr=MemberPrevious(Item);
		*prev_ptr=last;
		*Last=Item;
		}
	inline VOID Insert(_item_t* Item, COMPARE_FN Compare) { Insert(&m_First, &m_Last, Item, Compare); }
	static VOID Insert(_item_t** First, _item_t** Last, _item_t* Item, COMPARE_FN Compare)
		{
		assert(First!=nullptr);
		assert(Last!=nullptr);
		assert(Item!=nullptr);
		assert(Compare!=nullptr);
		auto current_ptr=First;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			assert(current!=Item);
			if(Compare(Item, current))
				{
				auto item_prev=MemberPrevious(Item);
				auto current_prev=MemberPrevious(current);
				*item_prev=*current_prev;
				*current_prev=Item;
				auto item_next=MemberNext(Item);
				*item_next=current;
				return;
				}
			auto current_next=MemberNext(current);
			if(*current_next==nullptr)
				{
				auto item_prev=MemberPrevious(Item);
				*item_prev=current;
				*current_next=Item;
				*Last=Item;
				return;
				}
			current_ptr=current_next;
			}
		*First=Item;
		*Last=Item;
		}
	inline _item_t* Remove(_item_t* Item) { return Remove(&m_First, &m_Last, Item); }
	static _item_t* Remove(_item_t** First, _item_t** Last, _item_t* Item)
		{
		assert(First!=nullptr);
		assert(Last!=nullptr);
		assert(Item!=nullptr);
		auto item_prev=MemberPrevious(Item);
		auto item_next=MemberNext(Item);
		auto prev=*item_prev;
		auto next=*item_next;
		if(prev)
			{
			auto prev_next=MemberNext(prev);
			*prev_next=Item;
			}
		else
			{
			*First=next;
			}
		if(next)
			{
			auto next_prev=MemberPrevious(next);
			*next_prev=prev;
			}
		else
			{
			*Last=prev;
			}
		*item_prev=nullptr;
		*item_next=nullptr;
		return next;
		}
	inline _item_t* RemoveFirst() { return RemoveFirst(&m_First, &m_Last); }
	static _item_t* RemoveFirst(_item_t** First, _item_t** Last)
		{
		assert(First!=nullptr);
		assert(Last!=nullptr);
		auto first=*First;
		if(!first)
			return nullptr;
		auto first_next=MemberNext(first);
		auto next=*first_next;
		*First=next;
		*first_next=nullptr;
		if(!next)
			*Last=nullptr;
		return first;
		}

private:
	// Common
	static inline _item_t** MemberNext(_item_t const* Item)noexcept
		{
		auto link=(Link<_item_t>*)((SIZE_T)Item+_member);
		return &link->Next;
		}
	static inline _item_t** MemberPrevious(_item_t const* Item)noexcept
		{
		auto link=(Link<_item_t>*)((SIZE_T)Item+_member);
		return &link->Previous;
		}
	_item_t* m_First=nullptr;
	_item_t* m_Last=nullptr;
};

}