//==============
// ring_buf.hpp
//==============

#pragma once


//=======
// Using
//=======

#include <assert.h>
#include <new>
#include <stdlib.h>
#include <utility>


//===========
// Namespace
//===========

namespace Storage {


//=============
// Ring-Buffer
//=============

template<class _item_t> class ring_buf
{
public:
	// Con-/Destructors
	ring_buf(): m_capacity(0), m_head(0), m_items(nullptr), m_tail(0) {}
	ring_buf(size_t capacity, size_t align=sizeof(size_t)): m_items(nullptr) { create(capacity, align); }
	~ring_buf()
		{
		clear();
		if(m_items)
			free(m_items);
		}
	void create(size_t capacity, size_t align=sizeof(size_t))
		{
		assert(!m_items);
		m_capacity=capacity;
		m_head=0;
		m_items=(_item_t*)aligned_alloc(align, capacity*sizeof(_item_t));
		m_tail=0;
		}

	// Common
	bool append(_item_t const& item)
		{
		if(full())
			return false;
		if(m_head==m_capacity)
			m_head=0;
		new (&m_items[m_head++]) _item_t(item);
		return true;
		}
	bool append(_item_t&& item)
		{
		if(full())
			return false;
		if(m_head==m_capacity)
			m_head=0;
		new (&m_items[m_head++]) _item_t(std::move(item));
		return true;
		}
	size_t available()const
		{
		if(m_head==m_tail)
			return m_head? m_capacity: 0;
		if(m_head>m_tail)
			return m_head-m_tail;
		return m_capacity-(m_tail-m_head);
		}
	inline size_t capacity()const
		{
		return m_capacity;
		}
	void clear()
		{
		size_t count=available();
		for(size_t u=0; u<count; u++)
			{
			m_items[m_tail].~_item_t();
			m_tail=(m_tail+1)%m_capacity;
			}
		m_head=0;
		m_tail=0;
		}
	bool consume(_item_t& item)
		{
		if(empty())
			return false;
		item=std::move(m_items[m_tail]);
		m_tail=(m_tail+1)%m_capacity;
		if((m_head%m_capacity)==m_tail)
			{
			m_head=0;
			m_tail=0;
			}
		return true;
		}
	size_t consume(_item_t* items, size_t count)
		{
		size_t copy=available();
		if(count>copy)
			count=copy;
		if(!count)
			return 0;
		for(size_t u=0; u<count; u++)
			{
			items[u]=std::move(m_items[m_tail]);
			m_tail=(m_tail+1)%m_capacity;
			}
		if((m_head%m_capacity)==m_tail)
			{
			m_head=0;
			m_tail=0;
			}
		return count;
		}
	inline bool empty()const
		{
		return m_head==m_tail==0;
		}
	inline bool full()const
		{
		return m_head==m_tail!=0;
		}
	size_t space()const
		{
		if(m_head==m_tail)
			{
			if(m_head==0)
				return m_capacity;
			return 0;
			}
		if(m_tail>m_head)
			return m_tail-m_head;
		return m_capacity-(m_head-m_tail);
		}

private:
	// Common
	size_t m_capacity;
	size_t m_head;
	_item_t* m_items;
	size_t m_tail;
};

}