//=========
// Array.h
//=========

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"


//===========
// Namespace
//===========

namespace Collections {


//======================
// Forward-Declarations
//======================

template <class _item_t, class _size_t> class ArrayIterator;


//=======
// Array
//=======

template <class _item_t, class _size_t=UINT>
class Array: public Object
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using Iterator=ArrayIterator<_item_t, _size_t>;
	using OutputStream=Storage::Streams::OutputStream;
	using StreamReader=Storage::Streams::StreamReader;
	using StreamWriter=Storage::Streams::StreamWriter;

	// Friends
	friend Iterator;

	// Con-/Destructors
	~Array()
		{
		if(m_Items)
			delete [] m_Items;
		}
	static inline Handle<Array> Create(_size_t Count) { return new Array(Count); }
	static inline Handle<Array> Create(Handle<Array> const& Copy) { return new Array(Copy); }

	// Access
	inline _item_t* Begin() { return m_Items; }
	inline _item_t const* Begin()const { return m_Items; }
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline _item_t& GetAt(_size_t Position) { return m_Items[Position]; }
	inline _item_t const& GetAt(_size_t Position)const { return m_Items[Position]; }
	inline _size_t GetCount()const { return m_Count; }
	SIZE_T PrintToStream(OutputStream* Stream, LPCSTR Format="%i")
		{
		SIZE_T size=0;
		StreamWriter writer(Stream);
		size+=writer.Print("[ ");
		for(_size_t u=0; u<m_Count; u++)
			{
			if(u>0)
				size+=writer.Print(", ");
			size+=writer.Print(Format, m_Items[u]);
			}
		size+=writer.Print(" ]");
		return size;
		}
	inline SIZE_T WriteToStream(OutputStream* Stream)
		{
		return Stream->Write(m_Items, m_Count*sizeof(_item_t));
		}

	// Modification
	virtual VOID Fill(_item_t const& Item)
		{
		for(_size_t u=0; u<m_Count; u++)
			m_Items[u]=Item;
		}
	inline SIZE_T ReadFromStream(InputStream* Stream)
		{
		return Stream->Read(m_Items, m_Count*sizeof(_item_t));
		}
	virtual VOID SetAt(_size_t Position, _item_t const& Item)
		{
		if(Position>=m_Count)
			throw OutOfRangeException();
		m_Items[Position]=Item;
		}

protected:
	// Common
	_size_t m_Count;
	_item_t* m_Items;

private:
	// Con-/Destructors
	Array(_size_t Count): m_Count(Count), m_Items(nullptr)
		{
		if(m_Count>0)
			m_Items=new _item_t[m_Count];
		}
	Array(Handle<Array> const& Copy): m_Count(Copy->GetCount())
		{
		m_Items=operator new(m_Count*sizeof(_item_t));
		for(_size_t u=0; u<m_Count; u++)
			new (&m_Items[u]) _item_t(Copy->GetAt(u));
		}
};


//==========
// Iterator
//==========

template <class _item_t, class _size_t>
class ArrayIterator: public Object
{
private:
	// Using
	using _array_t=Array<_item_t, _size_t>;

	// Friends
	friend _array_t;

public:
	// Access
	_item_t GetCurrent()const
		{
		if(!HasCurrent())
			return _item_t();
		return m_Array->GetAt(m_Position);
		}
	inline BOOL HasCurrent()const { return m_Position<m_Array->GetCount(); }
	BOOL MoveNext()
		{
		m_Position++;
		return HasCurrent();
		}
	BOOL MovePrevious()
		{
		if(m_Position==0)
			{
			m_Position=m_Array->GetCount();
			return false;
			}
		m_Position--;
		return true;
		}

private:
	// Con-/Destructors
	ArrayIterator(Handle<_array_t> Array, _size_t Position): m_Array(Array), m_Position(Position) {}

	// Common
	Handle<_array_t> m_Array;
	_size_t m_Position;
};

}