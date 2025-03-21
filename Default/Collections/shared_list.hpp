//=================
// shared_list.hpp
//=================

// Thread-safe implementation of an ordered list
// Items can be inserted and removed in constant low time

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// http://github.com/svenbieg/Clusters


#pragma once


//=======
// Using
//=======

#include "list.hpp"
#include "shared_cluster.hpp"


//===========
// Namespace
//===========

namespace Collections {


//=============
// Shared List
//=============

template <typename _item_t, typename _size_t=uint32_t, uint16_t _group_size=10>
class shared_list: public iterable_shared_cluster<list_traits<_item_t, _size_t, _group_size>>
{
public:
	// Using
	using _traits_t=list_traits<_item_t, _size_t, _group_size>;
	using _cluster_t=typename _traits_t::cluster_t;

	// Con-/Destructors
	shared_list() {}

	// Access
	inline bool contains(_item_t const& item)
		{
		std::shared_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::contains(item);
		}
	inline _size_t get_many(_size_t position, _item_t* items, _size_t count)
		{
		std::shared_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::get_many(position, items, count);
		}
	inline bool index_of(_item_t const& item, _size_t* position)
		{
		std::shared_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::index_of(item, position);
		}

	// Modification
	inline bool add(_item_t const& item)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::add(item);
		}
	inline void append(_item_t const& item)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		_cluster_t::append(item);
		}
	inline void append(_item_t const* items, _size_t count)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		_cluster_t::append(items, count);
		}
	inline bool insert_at(_size_t position, _item_t const& item)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::insert_at(position, item);
		}
	inline bool remove(_item_t const& item)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::remove(item);
		}
	inline bool set_at(_size_t position, _item_t const& item)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::set_at(position, item);
		}
	inline _size_t set_many(_size_t position, _item_t const* items, _size_t count)
		{
		std::unique_lock<std::shared_mutex> lock(this->m_mutex);
		return _cluster_t::set_many(position, items, count);
		}
};

}