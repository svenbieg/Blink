//=======
// Map.h
//=======

#pragma once


//=======
// Using
//=======

#include "shared_map.hpp"


//===========
// Namespace
//===========

namespace Collections {


//======================
// Forward-Declarations
//======================

template <typename _key_t, typename _value_t, typename _size_t, WORD _group_size> class MapIterator;
template <typename _key_t, typename _value_t, typename _size_t, WORD _group_size> class ConstMapIterator;


//=====
// Map
//=====

template <typename _key_t, typename _value_t, typename _size_t=UINT, WORD _group_size=10>
class Map: public Object
{
private:
	// Using
	using _map_t=Map<_key_t, _value_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=find_func;
	using Iterator=MapIterator<_key_t, _value_t, _size_t, _group_size>;
	using ConstIterator=ConstMapIterator<_key_t, _value_t, _size_t, _group_size>;

	// Friends
	friend Iterator;
	friend ConstIterator;

	// Con-/Destructors
	Map() {}
	Map(_map_t* Map)
		{
		if(Map)
			m_Map.copy_from(Map->m_Map);
		}

	// Access
	inline BOOL Contains(_key_t const& Key) { return m_Map.contains(Key); }
	inline Handle<Iterator> Find(_key_t const& Key, FindFunction Function=FindFunction::equal)
		{
		auto it=new Iterator(this, -2);
		it->Find(Key, Function);
		return it;
		}
	inline Handle<ConstIterator> FindConst(_key_t const& Key, FindFunction Function=FindFunction::equal)
		{
		auto it=new ConstIterator(this, -2);
		it->Find(Key, Function);
		return it;
		}
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline Handle<Iterator> FirstConst() { return new ConstIterator(this, 0); }
	inline _value_t Get(_key_t const& Key) { return m_Map.get(Key); }
	inline _size_t GetCount() { return m_Map.get_count(); }
	inline Handle<Iterator> Last()
		{
		auto it=new Iterator(this, -2);
		it->Last();
		return it;
		}
	inline Handle<Iterator> LastConst()
		{
		auto it=new ConstIterator(this, -2);
		it->Last();
		return it;
		}
	inline BOOL TryGet(_key_t const& Key, _value_t* Value) { return m_Map.try_get(Key, Value); }

	// Modification
	template <typename _key_param_t, typename _value_param_t> BOOL Add(_key_param_t&& Key, _value_param_t&& Value, BOOL Notify=true)
		{
		if(m_Map.add(std::forward<_key_param_t>(Key), std::forward<_value_param_t>(Value)))
			{
			if(Notify)
				{
				Added(this, Key, Value);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	Event<Map, _key_t, _value_t> Added;
	Event<Map> Changed;
	BOOL Clear(BOOL Notify=true)
		{
		if(m_Map.clear())
			{
			if(Notify)
				Changed(this);
			return true;
			}
		return false;
		}
	BOOL Remove(_key_t const& Key, BOOL Notify=true)
		{
		if(!Notify)
			return m_Map.remove(Key);
		auto it=m_Map.find(Key);
		if(!it.has_current())
			return false;
		_value_t value=it->get_value();
		it.remove_current();
		Removed(this, Key, value);
		Changed(this);
		return true;
		}
	BOOL RemoveAt(_size_t Position, BOOL Notify=true)
		{
		if(!Notify)
			return m_Map.remove_at(Position);
		auto it=m_Map.begin(Position);
		if(!it.has_current())
			return false;
		_key_t key=it->get_key();
		_value_t value=it->get_value();
		it.remove_current();
		Removed(this, key, value);
		Changed(this);
		return true;
		}
	Event<Map, _key_t, _value_t> Removed;
	template <typename _key_param_t, typename _value_param_t> BOOL Set(_key_param_t&& Key, _value_param_t&& Value, BOOL Notify=true)
		{
		if(m_Map.set(std::forward<_key_param_t>(Key), std::forward<_value_param_t>(Value)))
			{
			if(Notify)
				Changed(this);
			return true;
			}
		return false;
		}

protected:
	// Common
	shared_map<_key_t, _value_t, _size_t, _group_size> m_Map;
};


//==========
// Iterator
//==========

template <typename _key_t, typename _value_t, typename _size_t, WORD _group_size>
class MapIterator: public Object
{
private:
	// Using
	using _map_t=Map<_key_t, _value_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=find_func;

	// Con-/Destructors
	MapIterator(_map_t* Map, _size_t Position): m_It(&Map->m_Map, Position), m_Map(Map) {}

	// Access
	inline _key_t GetKey()const { return m_It->get_key(); }
	inline _value_t GetValue()const { return m_It->get_value(); }
	inline BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	inline BOOL Find(_key_t const& Key, FindFunction Function=FindFunction::equal) { return m_It.find(Key, Function); }
	inline BOOL First() { return m_It.begin(); }
	inline _size_t GetPosition() { return m_It.get_position(); }
	inline BOOL Last() { return m_It.rbegin(); }
	inline BOOL MoveNext() { return m_It.move_next(); }
	inline BOOL MovePrevious() { return m_It.move_previous(); }
	
	// Modification
	BOOL RemoveCurrent(BOOL Notify=true)
		{
		if(!Notify)
			return m_It.remove_current();
		if(!m_It.has_current())
			return false;
		_key_t id=m_It->get_key();
		_value_t item=m_It->get_value();
		m_It.remove_current();
		m_Map->Removed(m_Map, id, item);
		m_Map->Changed(m_Map);
		return true;
		}
	template <typename _value_param_t> inline VOID SetValue(_value_param_t&& Value)
		{
		m_It->set_value(std::forward<_value_param_t>(Value));
		}

private:
	// Common
	typename shared_map<_key_t, _value_t, _size_t, _group_size>::iterator m_It;
	Handle<_map_t> m_Map;
};

template <typename _key_t, typename _value_t, typename _size_t, WORD _group_size>
class ConstMapIterator: public Object
{
private:
	// Using
	using _map_t=Map<_key_t, _value_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=find_func;

	// Con-/Destructors
	ConstMapIterator(_map_t* Map, _size_t Position): m_It(&Map->m_Map, Position), m_Map(Map) {}

	// Access
	inline _key_t GetKey()const { return m_It->get_key(); }
	inline _value_t GetValue()const { return m_It->get_value(); }
	inline BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	inline BOOL Find(_key_t const& Key, FindFunction Function=FindFunction::equal) { return m_It.find(Key, Function); }
	inline BOOL First() { return m_It.begin(); }
	inline _size_t GetPosition() { return m_It.get_position(); }
	inline BOOL Last() { return m_It.rbegin(); }
	inline BOOL MoveNext() { return m_It.move_next(); }
	inline BOOL MovePrevious() { return m_It.move_previous(); }
	
private:
	// Common
	typename shared_map<_key_t, _value_t, _size_t, _group_size>::const_iterator m_It;
	Handle<_map_t> m_Map;
};

}