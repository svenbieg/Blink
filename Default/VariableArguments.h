//=====================
// VariableArguments.h
//=====================

#pragma once


//=======
// Using
//=======

#include "UnknownClass.h"


//====================
// Variable Arguments
//====================

class VariableArguments
{
public:
	// Con-/Destructors
	VariableArguments(UnknownClass* Arguments, UINT Count): m_Arguments(Arguments), m_Count(Count) {}

	// Access
	template <class _item_t> BOOL GetAt(UINT Position, _item_t& Item)const
		{
		if(Position>=m_Count)
			return false;
		return m_Arguments[Position].Get(Item);
		}
	UINT GetCount()const { return m_Count; }

private:
	// Common
	UnknownClass* m_Arguments;
	UINT m_Count;
};
