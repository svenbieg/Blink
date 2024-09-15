//=======
// Map.h
//=======

#pragma once


//=======
// Using
//=======

#include "Clusters/shared_map.hpp"


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
	using FindFunction=Clusters::find_func;
	using Iterator=MapIterator<_key_t, _value_t, _size_t, _group_size>;
	using ConstIterator=ConstMapIterator<_key_t, _value_t, _size_t, _group_size>;

	// Friends
	friend Iterator;
	friend ConstIterator;

	// Con-/Destructors
	Map() {}
	Map(nullptr_t) {}
	Map(_map_t* Map): cMap(Map->cMap) {}

	// Access
	inline BOOL Contains(_key_t const& Key) { return cMap.contains(Key); }
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
	inline _value_t Get(_key_t const& Key) { return cMap.get(Key); }
	inline _size_t GetCount() { return cMap.get_count(); }
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
	inline BOOL TryGet(_key_t const& Key, _value_t* Value) { return cMap.try_get(Key, Value); }

	// Modification
	template <typename _key_param_t, typename _value_param_t> BOOL Add(_key_param_t&& Key, _value_param_t&& Value, BOOL Notify=true)
		{
		if(cMap.add(std::forward<_key_param_t>(Key), std::forward<_value_param_t>(Value)))
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
		if(cMap.clear())
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
			return cMap.remove(Key);
		auto it=cMap.find(Key);
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
			return cMap.remove_at(Position);
		auto it=cMap.begin(Position);
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
		if(cMap.set(std::forward<_key_param_t>(Key), std::forward<_value_param_t>(Value)))
			{
			if(Notify)
				Changed(this);
			return true;
			}
		return false;
		}

protected:
	// Common
	Clusters::shared_map<_key_t, _value_t, _size_t, _group_size> cMap;
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
	using FindFunction=Clusters::find_func;

	// Con-/Destructors
	MapIterator(_map_t* Map, _size_t Position): cIt(&Map->cMap, Position), hMap(Map) {}

	// Access
	inline _key_t GetKey()const { return cIt->get_key(); }
	inline _value_t GetValue()const { return cIt->get_value(); }
	inline BOOL HasCurrent()const { return cIt.has_current(); }

	// Navigation
	inline BOOL Find(_key_t const& Key, FindFunction Function=FindFunction::equal) { return cIt.find(Key, Function); }
	inline BOOL First() { return cIt.begin(); }
	inline _size_t GetPosition() { return cIt.get_position(); }
	inline BOOL Last() { return cIt.rbegin(); }
	inline BOOL MoveNext() { return cIt.move_next(); }
	inline BOOL MovePrevious() { return cIt.move_previous(); }
	
	// Modification
	BOOL RemoveCurrent(BOOL Notify=true)
		{
		if(!Notify)
			return cIt.remove_current();
		if(!cIt.has_current())
			return false;
		_key_t id=cIt->get_key();
		_value_t item=cIt->get_value();
		cIt.remove_current();
		hMap->Removed(hMap, id, item);
		hMap->Changed(hMap);
		return true;
		}
	template <typename _value_param_t> inline VOID SetValue(_value_param_t&& Value)
		{
		cIt->set_value(std::forward<_value_param_t>(Value));
		}

private:
	// Common
	typename Clusters::shared_map<_key_t, _value_t, _size_t, _group_size>::iterator cIt;
	Handle<_map_t> hMap;
};

template <typename _key_t, typename _value_t, typename _size_t, WORD _group_size>
class ConstMapIterator: public Object
{
private:
	// Using
	using _map_t=Map<_key_t, _value_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=Clusters::find_func;

	// Con-/Destructors
	ConstMapIterator(_map_t* Map, _size_t Position): cIt(&Map->cMap, Position), hMap(Map) {}

	// Access
	inline _key_t GetKey()const { return cIt->get_key(); }
	inline _value_t GetValue()const { return cIt->get_value(); }
	inline BOOL HasCurrent()const { return cIt.has_current(); }

	// Navigation
	inline BOOL Find(_key_t const& Key, FindFunction Function=FindFunction::equal) { return cIt.find(Key, Function); }
	inline BOOL First() { return cIt.begin(); }
	inline _size_t GetPosition() { return cIt.get_position(); }
	inline BOOL Last() { return cIt.rbegin(); }
	inline BOOL MoveNext() { return cIt.move_next(); }
	inline BOOL MovePrevious() { return cIt.move_previous(); }
	
private:
	// Common
	typename Clusters::shared_map<_key_t, _value_t, _size_t, _group_size>::const_iterator cIt;
	Handle<_map_t> hMap;
};

}