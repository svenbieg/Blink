//=========
// Index.h
//=========

#pragma once


//=======
// Using
//=======

#include "Clusters/shared_index.hpp"


//===========
// Namespace
//===========

namespace Collections {


//======================
// Forward-Declarations
//======================

template <typename _id_t, typename _size_t, WORD _group_size> class IndexIterator;
template <typename _id_t, typename _size_t, WORD _group_size> class ConstIndexIterator;


//=======
// Index
//=======

template <typename _id_t, typename _size_t=UINT, WORD _group_size=10>
class Index: public Object
{
private:
	// Using
	using _index_t=Index<_id_t, _size_t, _group_size>;

public:
	// Types
	using FindFunction=Clusters::find_func;
	using Iterator=IndexIterator<_id_t, _size_t, _group_size>;
	using ConstIterator=ConstIndexIterator<_id_t, _size_t, _group_size>;

	// Friends
	friend Iterator;
	friend ConstIterator;

	// Con-/Destructors
	Index() {}
	Index(nullptr_t) {}
	Index(_index_t* Index): cIndex(Index->cIndex) {}

	// Access
	inline Handle<Iterator> At(UINT Position) { return new Iterator(this, Position); }
	inline BOOL Contains(_id_t const& Id) { return cIndex.contains(Id); }
	inline Handle<Iterator> Find(_id_t const& Id, FindFunction Function=FindFunction::equal)
		{
		auto it=new Iterator(this, -2);
		it->Find(Id, Function);
		return it;
		}
	inline Handle<ConstIterator> FindConst(_id_t const& Id, FindFunction Function=FindFunction::equal)
		{
		auto it=new ConstIterator(this, -2);
		it->Find(Id, Function);
		return it;
		}
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline Handle<ConstIterator> FirstConst() { return new ConstIterator(this, 0); }
	inline _id_t GetAt(_size_t Position) { return cIndex.get_at(Position); }
	inline _size_t GetCount() { return cIndex.get_count(); }
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
	template <typename _id_param_t> BOOL Add(_id_param_t&& Id, BOOL Notify=true)
		{
		if(cIndex.add(std::forward<_id_param_t>(Id)))
			{
			if(Notify)
				{
				Added(this, Id);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	Event<Index, _id_t> Added;
	Event<Index> Changed;
	BOOL Clear(BOOL Notify=true)
		{
		if(cIndex.clear())
			{
			if(Notify)
				Changed(this);
			return true;
			}
		return false;
		}
	BOOL Remove(_id_t const& Id, BOOL Notify=true)
		{
		if(cIndex.remove(Id))
			{
			if(Notify)
				{
				Removed(this, Id);
				Changed(this);
				}
			return true;
			}
		return false;
		}
	Event<Index, _id_t> Removed;

private:
	// Common
	Clusters::shared_index<_id_t, _size_t, _group_size> cIndex;
};


//==========
// Iterator
//==========

template <typename _id_t, typename _size_t, WORD _group_size>
class IndexIterator: public Object
{
private:
	// Using
	using _index_t=Index<_id_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=Clusters::find_func;

	// Con-/Destructors
	IndexIterator(_index_t* Index, _size_t Position): cIt(&Index->cIndex, Position), hIndex(Index) {}

	// Access
	inline _id_t GetCurrent()const { return *cIt; }
	inline BOOL HasCurrent()const { return cIt.has_current(); }

	// Navigation
	inline BOOL Find(_id_t const& Id, FindFunction Function=FindFunction::equal) { return cIt.find(Id, Function); }
	inline BOOL MoveNext() { return cIt.move_next(); }
	inline BOOL MovePrevious() { return cIt.move_previous(); }

	// Modification
	BOOL RemoveCurrent(BOOL Notify=true)
		{
		if(!Notify)
			return cIt.remove_current();
		if(!cIt.has_current())
			return false;
		_id_t id=cIt.get_current_id();
		cIt.remove_current();
		hIndex->Removed(hIndex, id);
		hIndex->Changed(hIndex);
		return true;
		}

private:
	// Common
	typename Clusters::shared_index<_id_t, _size_t, _group_size>::iterator cIt;
	Handle<_index_t> hIndex;
};


template <typename _id_t, typename _size_t, WORD _group_size>
class ConstIndexIterator: public Object
{
private:
	// Using
	using _index_t=Index<_id_t, _size_t, _group_size>;

public:
	// Using
	using FindFunction=Clusters::find_func;

	// Con-/Destructors
	ConstIndexIterator(_index_t* Index, _size_t Position): cIt(&Index->cIndex, Position), hIndex(Index) {}

	// Access
	inline _id_t GetCurrent()const { return *cIt; }
	inline BOOL HasCurrent()const { return cIt.has_current(); }

	// Navigation
	inline BOOL Find(_id_t const& Id, FindFunction Function=FindFunction::equal) { return cIt.find(Id, Function); }
	inline BOOL MoveNext() { return cIt.move_next(); }
	inline BOOL MovePrevious() { return cIt.move_previous(); }

private:
	// Common
	typename Clusters::shared_index<_id_t, _size_t, _group_size>::const_iterator cIt;
	Handle<_index_t> hIndex;
};

}