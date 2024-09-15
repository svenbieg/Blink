//=============
// Container.h
//=============

#pragma once


//===========
// Namespace
//===========

namespace Collections {


//======================
// Forward-Declarations
//======================

template <typename _item_t, typename _size_t> class ContainerIterator;


//===========
// Container
//===========

template <typename _item_t, typename _size_t>
class Container: public Object
{
public:
	// Types
	using Iterator=ContainerIterator<_item_t, _size_t>;

	// Friends
	friend Iterator;

	// Access
	virtual Handle<Iterator> At(UINT Position)=0;
	virtual Handle<Iterator> First()=0;
	virtual _item_t GetAt(_size_t Position)=0;
	virtual _size_t GetCount()=0;
	virtual Handle<Iterator> Last()=0;

	// Modification
	Event<Container, _item_t> Added;
	Event<Container> Changed;
	virtual VOID Clear(BOOL Notify=true)=0;
	Event<Container, _id_t> Removed;

protected:
	// Con-/Destructors
	Container() {}
};


//==========
// Iterator
//==========

template <typename _item_t, typename _size_t>
class ContainerIterator: public Object
{
private:
	// Using
	using _container_t=Container<_item_t, _size_t>;

public:
	// Access
	virtual _item_t GetCurrent()const=0;
	virtual BOOL HasCurrent()const=0;

	// Navigation
	virtual BOOL MoveNext()=0;
	virtual BOOL MovePrevious()=0;

	// Modification
	virtual BOOL RemoveCurrent(BOOL Notify=true)=0;

protected:
	// Con-/Destructors
	ContainerIterator() {}
};

}