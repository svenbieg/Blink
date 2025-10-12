//==============
// ring_buf.hpp
//==============

#pragma once


//=======
// Using
//=======

#include <assert.h>
#include <new>
#include <utility>


//===========
// Namespace
//===========

namespace Storage {


//=============
// Ring-Buffer
//=============

template<class _item_t, class _size_t=UINT> class ring_buf
{
public:
	// Con-/Destructors
	ring_buf(): m_capacity(0), m_head(0), m_items(nullptr), m_tail(0) {}
	ring_buf(_size_t capacity, size_t align=sizeof(size_t)): m_items(nullptr) { create(capacity, align); }
	~ring_buf()
		{
		if(m_items)
			free(m_items);
		}
	void create(_size_t capacity, size_t align=sizeof(size_t))
		{
		if(m_items)
			free(m_items);
		m_capacity=capacity;
		m_head=0;
		m_items=(_item_t*)aligned_alloc(align, capacity*sizeof(_item_t));
		m_tail=0;
		}

	// Access
	inline operator bool()const { return available()>0; }
	inline _size_t available()const { return m_head-m_tail; }
	inline _size_t capacity()const { return m_capacity; }
	void consume(_item_t* item_ptr)
		{
		if(m_head==m_tail)
			throw BufferOverrunException();
		*item_ptr=std::move(m_items[m_tail++]);
		if(m_head==m_tail)
			{
			m_head=0;
			m_tail=0;
			}
		}
	_size_t consume(_item_t* items, _size_t count)
		{
		UINT pos=0;
		for(; pos<count; pos++)
			{
			if(m_head==m_tail)
				{
				m_head=0;
				m_tail=0;
				break;
				}
			items[pos]=std::move(m_items[m_tail++]);
			}
		return pos;
		}
	inline bool empty()const { return m_head==m_tail; }
	inline bool full()const { return space()==0; }
	inline _size_t space()const { return m_capacity-available(); }

	// Modification
	void append(_item_t const& item)
		{
		if(full())
			throw BufferOverrunException();
		new (&m_items[m_head%m_capacity]) _item_t(item);
		m_head++;
		}
	void append(_item_t&& item)
		{
		if(full())
			throw BufferOverrunException();
		new (&m_items[m_head%m_capacity]) _item_t(std::move(item));
		m_head++;
		}
	void clear()
		{
		while(m_tail<m_head)
			{
			m_items[m_tail%m_capacity].~_item_t();
			m_tail++;
			}
		m_head=0;
		m_tail=0;
		}

private:
	// Common
	_size_t m_capacity;
	_size_t m_head;
	_item_t* m_items;
	_size_t m_tail;
};

}