//==========
// Vector.h
//==========

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

template <class _item_t, class _size_t> class VectorIterator;


//========
// Vector
//========

template <class _item_t, class _size_t=UINT>
class Vector: public Object
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;
	using StreamReader=Storage::Streams::StreamReader;
	using StreamWriter=Storage::Streams::StreamWriter;

	// Iterator
	using Iterator=VectorIterator<_item_t, _size_t>;
	friend Iterator;

	// Con-/Destructors
	Vector(_size_t Count): m_Count(Count), m_Items(nullptr)
		{
		if(m_Count>0)
			m_Items=new _item_t[m_Count];
		}
	Vector(Handle<Vector> Vector): m_Count(Vector->GetCount())
		{
		m_Items=operator new(m_Count*sizeof(_item_t));
		for(_size_t u=0; u<m_Count; u++)
			new (&m_Items[u]) _item_t(Vector->GetAt(u));
		}
	~Vector()
		{
		if(m_Items)
			delete [] m_Items;
		}

	// Access
	inline _item_t* Begin() { return m_Items; }
	inline _item_t const* Begin()const { return m_Items; }
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline _item_t& GetAt(SIZE_T Position) { return m_Items[Position]; }
	inline _item_t const& GetAt(SIZE_T Position)const { return m_Items[Position]; }
	inline _size_t GetCount()const { return m_Count; }
	SIZE_T PrintToStream(Handle<OutputStream> Stream, LPCSTR Format="%i")
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
	inline SIZE_T WriteToStream(Handle<OutputStream> Stream)
		{
		return Stream->Write(m_Items, m_Count*sizeof(_item_t));
		}

	// Modification
	virtual VOID Clear()
		{
		MemoryHelper::Fill(m_Items, m_Count*sizeof(_item_t));
		}
	virtual VOID Fill(_item_t const& Item)
		{
		for(SIZE_T u=0; u<m_Count; u++)
			m_Items[u]=Item;
		}
	inline SIZE_T ReadFromStream(Handle<InputStream> Stream)
		{
		return Stream->Read(m_Items, m_Count*sizeof(_item_t));
		}
	virtual BOOL SetAt(_size_t Position, _item_t const& Item)
		{
		if(Position>=m_Count)
			return false;
		m_Items[Position]=Item;
		return true;
		}

protected:
	// Common
	_size_t m_Count;
	_item_t* m_Items;
};


//==========
// Iterator
//==========

template <class _item_t, class _size_t>
class VectorIterator: public Object
{
private:
	// Using
	using _vector_t=Vector<_item_t, _size_t>;

public:
	// Con-/Destructors
	VectorIterator(Handle<_vector_t> Vector, _size_t Position): m_Position(Position), m_Vector(Vector) {}

	// Access
	_item_t GetCurrent()const
		{
		if(!HasCurrent())
			return _item_t();
		return m_Vector->GetAt(m_Position);
		}
	inline BOOL HasCurrent()const { return m_Position<m_Vector->GetCount(); }
	BOOL MoveNext()
		{
		m_Position++;
		return HasCurrent();
		}
	BOOL MovePrevious()
		{
		if(m_Position==0)
			{
			m_Position=m_Vector->GetCount();
			return false;
			}
		m_Position--;
		return true;
		}

private:
	_size_t m_Position;
	Handle<_vector_t> m_Vector;
};

}