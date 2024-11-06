//========
// List.h
//========

#pragma once


//=======
// Using
//=======

#include "Clusters/shared_list.hpp"


//===========
// Namespace
//===========

namespace Collections {


//======================
// Forward-Declarations
//======================

template <typename _item_t, typename _size_t, WORD _group_size> class ListIterator;
template <typename _item_t, typename _size_t, WORD _group_size> class ConstListIterator;


//======
// List
//======

template <typename _item_t, typename _size_t=UINT, WORD _group_size=10>
class List: public Object
{
private:
	// Using
	using _list_t=List<_item_t, _size_t, _group_size>;

public:
	// Using
	using Iterator=ListIterator<_item_t, _size_t, _group_size>;
	using ConstIterator=ConstListIterator<_item_t, _size_t, _group_size>;

	// Friends
	friend Iterator;
	friend ConstIterator;

	// Con-/Destructors
	List() {}
	List(_list_t* List)
		{
		if(List)
			m_List.copy_from(List->m_List);
		}

	// Access
	inline Handle<Iterator> At(_size_t Position) { return new Iterator(this, Position); }
	inline Handle<ConstIterator> AtConst(_size_t Position) { return new ConstIterator(this, Position); }
	inline BOOL Contains(_item_t const& Item) { return m_List.contains(Item); }
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline Handle<ConstIterator> FirstConst() { return new ConstIterator(this, 0); }
	inline _item_t GetAt(_size_t Position)
		{
		_size_t count=m_List.get_count();
		if(Position>=count)
			return _item_t();
		return m_List.get_at(Position);
		}
	inline _size_t GetCount() { return m_List.get_count(); }
	inline BOOL IndexOf(_item_t const& Item, _size_t* Position) { return m_List.index_of(Item, Position); }
	inline Handle<Iterator> Last()
		{
		auto it=new Iterator(this, -2);
		it->Last();
		return it;
		}
	inline Handle<ConstIterator> LastConst()
		{
		auto it=new ConstIterator(this, -2);
		it->Last();
		return it;
		}

	// Modification
	template <typename _item_param_t> BOOL Add(_item_param_t&& Item, BOOL Notify=true)
		{
		if(m_List.add(std::forward<_item_param_t>(Item)))
			{
			if(Notify)
				{
				Added(this, Item);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	Event<List, _item_t> Added;
	template <typename _item_param_t> VOID Append(_item_param_t&& Item, BOOL Notify=true)
		{
		m_List.append(std::forward<_item_param_t>(Item));
		if(Notify)
			{
			Added(this, Item);
			Changed(this);
			}
		}
	Event<List> Changed;
	BOOL Clear(BOOL Notify=true)
		{
		if(m_List.clear())
			{
			if(Notify)
				Changed(this);
			return true;
			}
		return false;
		}
	template <typename _item_param_t> BOOL InsertAt(_size_t Position, _item_param_t&& Item, BOOL Notify=true)
		{
		if(m_List.insert_at(Position, std::forward<_item_param_t>(Item)))
			{
			if(Notify)
				{
				Added(this, Item);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	template <typename _item_param_t> BOOL Remove(_item_param_t&& Item, BOOL Notify=true)
		{
		if(m_List.remove(std::forward<_item_param_t>(Item)))
			{
			if(Notify)
				{
				Removed(this, Item);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	VOID RemoveAll()
		{
		BOOL any=false;
		for(auto it=m_List.begin(); it.has_current(); )
			{
			_item_t item=*it;
			it.remove_current();
			Removed(this, item);
			any=true;
			}
		if(any)
			Changed(this);
		}
	BOOL RemoveAt(_size_t Position, BOOL Notify=true)
		{
		if(!Notify)
			return m_List.remove_at(Position);
		auto it=m_List.begin(Position);
		if(!it.has_current())
			return false;
		_item_t item=*it;
		it.remove_current();
		Removed(this, item);
		Changed(this);
		return true;
		}
	Event<List, _item_t> Removed;
	template <typename _item_param_t> BOOL SetAt(_size_t Position, _item_param_t&& Item, BOOL Notify=true)
		{
		if(m_List.set_at(Position, std::forward<_item_param_t>(Item)))
			{
			Changed(this);
			return true;
			}
		return false;
		}

protected:
	// Common
	Clusters::shared_list<_item_t, _size_t, _group_size> m_List;
};


//==========
// Iterator
//==========

template <typename _item_t, typename _size_t, WORD _group_size>
class ListIterator: public Object
{
private:
	// Using
	using _list_t=List<_item_t, _size_t, _group_size>;

public:
	// Con-/Destructors
	ListIterator(_list_t* List, _size_t Position): m_It(&List->m_List, Position), hList(List) {}

	// Access
	_item_t& GetCurrent()
		{
		if(!m_It.has_current())
			return cItem;
		return *m_It;
		}
	BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	BOOL First() { return m_It.begin(); }
	_size_t GetPosition() { return m_It.get_position(); }
	BOOL Last() { return m_It.rbegin(); }
	BOOL Move(BOOL Forward, BOOL Repeat)
		{
		if(Forward)
			{
			if(!m_It.move_next())
				{
				if(Repeat)
					return m_It.begin();
				return false;
				}
			}
		else
			{
			if(!m_It.move_previous())
				{
				if(Repeat)
					return m_It.rbegin();
				return false;
				}
			}
		return true;
		}
	BOOL MoveNext() { return m_It.move_next(); }
	BOOL MovePrevious() { return m_It.move_previous(); }

	// Modification
	BOOL RemoveCurrent(BOOL Notify=true)
		{
		if(!Notify)
			return m_It.remove_current();
		if(!m_It.has_current())
			return false;
		_item_t item=m_It.get_current();
		m_It.remove_current();
		hList->Removed(hList, item);
		hList->Changed(hList);
		return true;
		}

private:
	// Common
	typename Clusters::shared_list<_item_t, _size_t, _group_size>::iterator m_It;
	static inline _item_t cItem=_item_t();
	Handle<_list_t> hList;
};

template <typename _item_t, typename _size_t, WORD _group_size>
class ConstListIterator: public Object
{
private:
	// Using
	using _list_t=List<_item_t, _size_t, _group_size>;

public:
	// Con-/Destructors
	ConstListIterator(_list_t* List, _size_t Position): m_It(&List->m_List, Position), hList(List) {}

	// Access
	_item_t& GetCurrent()const { return *m_It; }
	BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	BOOL First() { return m_It.begin(); }
	_size_t GetPosition() { return m_It.get_position(); }
	BOOL Last() { return m_It.rbegin(); }
	BOOL MoveNext() { return m_It.move_next(); }
	BOOL MovePrevious() { return m_It.move_previous(); }

private:
	// Common
	typename Clusters::shared_list<_item_t, _size_t, _group_size>::const_iterator m_It;
	Handle<_list_t> hList;
};

}