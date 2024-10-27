//============
// Property.h
//============

#pragma once


//==========
// Property
//==========

template <class _owner_t, class _item_t>
class Property
{
public:
	// Con-/Destructors
	Property(_owner_t* Owner, _item_t Value=_item_t()): m_Owner(Owner), tValue(Value) {}
	Property(Property const&)=delete;
	Property(Property&&)=delete;

	// Access
	//inline operator bool()const { return tValue; }
	inline operator _item_t const&()const { return tValue; }
	inline _item_t const& Get()const { return tValue; }

	// Modification
	inline Property& operator=(_item_t Value)
		{
		Set(Value);
		return *this;
		}
	inline Property& operator=(Property const& Property)
		{
		return operator=(Property.tValue);
		}
	Event<_owner_t, _item_t> Changed;
	VOID Set(_item_t const& Value, BOOL Notify=true)
		{
		if(tValue==Value)
			return;
		tValue=Value;
		if(Notify)
			Changed(m_Owner, Value);
		}

private:
	// Common
	_owner_t* m_Owner;
	_item_t tValue;
};
