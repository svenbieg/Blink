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
	Vector(Handle<Vector> Vector): uCount(Vector->GetCount())
		{
		pItems=operator new(uCount*sizeof(_item_t));
		for(_size_t u=0; u<uCount; u++)
			new (&pItems[u]) _item_t(Vector->GetAt(u));
		}
	Vector(_size_t Count): pItems(nullptr), uCount(Count)
		{
		if(uCount>0)
			pItems=new _item_t[uCount];
		}
	~Vector()
		{
		if(pItems)
			delete [] pItems;
		}

	// Access
	inline _item_t* Begin() { return pItems; }
	inline _item_t const* Begin()const { return pItems; }
	inline Handle<Iterator> First() { return new Iterator(this, 0); }
	inline _item_t& GetAt(SIZE_T Position) { return pItems[Position]; }
	inline _item_t const& GetAt(SIZE_T Position)const { return pItems[Position]; }
	inline _size_t GetCount()const { return uCount; }
	SIZE_T PrintToStream(Handle<OutputStream> Stream, LPCSTR Format="%i")
		{
		SIZE_T size=0;
		StreamWriter writer(Stream);
		size+=writer.Print("[ ");
		for(_size_t u=0; u<uCount; u++)
			{
			if(u>0)
				size+=writer.Print(", ");
			size+=writer.Print(Format, pItems[u]);
			}
		size+=writer.Print(" ]");
		return size;
		}
	inline SIZE_T WriteToStream(Handle<OutputStream> Stream)
		{
		return Stream->Write(pItems, uCount*sizeof(_item_t));
		}

	// Modification
	virtual VOID Clear()
		{
		ZeroMemory(pItems, uCount*sizeof(_item_t));
		}
	virtual VOID Fill(_item_t const& Item)
		{
		for(SIZE_T u=0; u<uCount; u++)
			pItems[u]=Item;
		}
	inline SIZE_T ReadFromStream(Handle<InputStream> Stream)
		{
		return Stream->Read(pItems, uCount*sizeof(_item_t));
		}
	virtual BOOL SetAt(_size_t Position, _item_t const& Item)
		{
		if(Position>=uCount)
			return false;
		pItems[Position]=Item;
		return true;
		}

protected:
	// Common
	_item_t* pItems;
	_size_t uCount;
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
	VectorIterator(Handle<_vector_t> Vector, _size_t Position): hVector(Vector), uPosition(Position) {}

	// Access
	_item_t GetCurrent()const
		{
		if(!HasCurrent())
			return _item_t();
		return hVector->GetAt(uPosition);
		}
	inline BOOL HasCurrent()const { return uPosition<hVector->GetCount(); }
	BOOL MoveNext()
		{
		uPosition++;
		return HasCurrent();
		}
	BOOL MovePrevious()
		{
		if(uPosition==0)
			{
			uPosition=hVector->GetCount();
			return false;
			}
		uPosition--;
		return true;
		}

private:
	Handle<_vector_t> hVector;
	_size_t uPosition;
};

}