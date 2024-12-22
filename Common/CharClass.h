//=============
// CharClass.h
//=============

#pragma once


//=======
// Using
//=======

#include "CharHelper.h"
#include "StringClass.h"
#include "Variable.h"


//======
// Char
//======

class Char: public TypedVariable<CHAR>
{
public:
	// Using
	using Language=Culture::Language;

	// Con-/Destructors
	Char(CHAR Value=0): TypedVariable(nullptr, Value) {}
	Char(WCHAR Value): TypedVariable(nullptr, CharHelper::ToAnsi(Value)) {}
	Char(Handle<String> Name, CHAR Value=0): TypedVariable(Name, Value) {}
	Char(Handle<String> Name, WCHAR Value): TypedVariable(Name, CharHelper::ToAnsi(Value)) {}

	// Access
	inline BOOL IsAlpha() { return CharHelper::IsAlpha(m_Value); }
	inline BOOL IsBreak() { return CharHelper::IsBreak(m_Value); }
	inline BOOL IsCapital() { return CharHelper::IsCapital(m_Value); }
	inline BOOL IsDigit(UINT Base=10) { return CharHelper::IsDigit(m_Value, Base); }
	inline BOOL IsSmall() { return CharHelper::IsSmall(m_Value); }
	inline BOOL IsSpecial() { return CharHelper::IsSpecial(m_Value); }
	Handle<String> ToString(LanguageCode Language)override { return new String("%c", Get()); }
};
