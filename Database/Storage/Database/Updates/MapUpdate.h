//=============
// MapUpdate.h
//=============

#pragma once


//=======
// Using
//=======

#include "Storage/Database/Updates/EntryUpdate.h"


//======================
// Forward-Declarations
//======================

namespace Storage
{
namespace Database
	{
	template <class _key_t, class _value_t, class _size_t, WORD _group_size> class Map;
	}
}


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {
		namespace Updates {


//=========
// Updates
//=========

enum class MapUpdateId: BYTE
{
None,
RootSet
};


//============
// Map-Update
//============

template <class _key_t, class _value_t, class _size_t, WORD _group_size>
class MapUpdate: public EntryUpdate
{
public:
	// Using
	using _map_t=Map<_key_t, _value_t, _size_t, _group_size>;

	// Friends
	friend _map_t;

	// Common
	static SIZE_T ReadFromStream(InputStream* Stream, _map_t* Map, EntryUpdate** Update=nullptr)
		{
		SIZE_T size=0;
		while(1)
			{
			WORD skip=0;
			size+=Stream->Read(&skip, sizeof(WORD));
			if(skip!=-1)
				{
				size+=Stream->Read(nullptr, skip);
				continue;
				}
			MapUpdateId update=MapUpdateId::None;
			size+=Stream->Read(&update, sizeof(MapUpdateId));
			}
		return 0;
		}
};

}}}