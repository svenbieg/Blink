//=======
// Map.h
//=======

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#pragma once


//=======
// Using
//=======

#include "Storage/Database/Updates/MapUpdate.h"
#include "Storage/Database/Entry.h"
#include "Storage/Block.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//=====
// Map
//=====

template <class _key_t, class _value_t, class _size_t=UINT, WORD _group_size=10>
class Map: public Entry
{
public:
	// Using
	using _update_t=Storage::Database::Updates::MapUpdate<_key_t, _value_t, _size_t, _group_size>;

	// Friends
	friend Object;

	// Common
	VOID Clear();
	BOOL Set(_key_t const& Key, _value_t const& Value);

protected:
	// Common
	SIZE_T ReadEntry(InputStream* Stream)override
		{
		SIZE_T size=0;
		size+=Entry::ReadEntry(Stream);
		size+=_update_t::ReadFromStream(Stream, this);
		return size;
		}
	SIZE_T WriteEntry(OutputStream* Stream)override
		{
		throw NotImplementedException();
		return 0;
		}

private:
	// Settings
	static constexpr UINT MAP_TYPE=ENTRY_TYPE('MAP');

	// Con-/Destructors
	Map(Database* Database): Entry(Database, -1, MAP_TYPE) {}
	Map(Database* Database, UINT Id): Entry(Database, Id, MAP_TYPE)
		{
		auto volume=GetVolume();
		auto block=Block::Create(volume, Id);
		ReadEntry(block);
		block->Skip();
		_update_t::ReadFromStream(block, this, &m_Update);
		m_Size=block->GetPosition();
		}
	static inline Handle<Map> Create(Database* Database, UINT Block=-1)
		{
		return Entry::Create<Map>(Database, Block);
		}

	// Group
	class Group: public Entry
		{
		public:
			// Common
			virtual _key_t const& GetFirstKey();
			virtual _key_t const& GetLastKey();
			virtual WORD GetLevel();
			virtual BOOL Set(_key_t const& Key, _value_t const& Value);
		};

	// Item-Group
	class ItemGroup: public Group
		{
		public:
			// Common
			_key_t const& GetFirstKey()override;
			_key_t const& GetLastKey()override;
			WORD GetLevel()override { return 0; }
			BOOL Set(_key_t const& Key, _value_t const& Value)override;
		};

	// Parent-Group
	class ParentGroup: public Group
		{
		public:
			// Common
			_key_t const& GetFirstKey()override;
			_key_t const& GetLastKey()override;
			WORD GetLevel()override;
			BOOL Set(_key_t const& Key, _value_t const& Value)override;
		};

	// Common
	Handle<Group> m_Root;
};

}}