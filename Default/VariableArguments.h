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
	VariableArguments(UnknownClass* Arguments, UINT Count): pArguments(Arguments), uCount(Count) {}

	// Access
	template <class _item_t> BOOL GetAt(UINT Position, _item_t& Item)const
		{
		if(Position>=uCount)
			return false;
		return pArguments[Position].Get(Item);
		}
	UINT GetCount()const { return uCount; }

private:
	// Common
	UnknownClass* pArguments;
	UINT uCount;
};
