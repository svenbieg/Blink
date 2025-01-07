//=============
// CharClass.h
//=============

#pragma once


//=======
// Using
//=======

#include "CharHelper.h"
#include "Variable.h"


//======
// Char
//======

class Char: public TypedVariable<WCHAR>
{
public:
	// Con-/Destructors
	static inline Handle<Char> Create(CHAR Value=0) { return new Char(nullptr, Value); }
	static inline Handle<Char> Create(WCHAR Value) { return new Char(nullptr, Value); }
	static inline Handle<Char> Create(Handle<String> Name, CHAR Value=0) { return new Char(Name, Value); }
	static inline Handle<Char> Create(Handle<String> Name, WCHAR Value) { return new Char(Name, Value); }

	// Access
	inline BOOL IsAlpha()const { return CharHelper::IsAlpha(m_Value); }
	inline BOOL IsBreak()const { return CharHelper::IsBreak(m_Value); }
	inline BOOL IsCapital()const { return CharHelper::IsCapital(m_Value); }
	inline BOOL IsDigit(UINT Base=10)const { return CharHelper::IsDigit(m_Value, Base); }
	inline BOOL IsSmall()const { return CharHelper::IsSmall(m_Value); }
	inline BOOL IsSpecial()const { return CharHelper::IsSpecial(m_Value); }
	Handle<String> ToString(LanguageCode Language)override { return String::Create("%c", Get()); }

private:
	// Con-/Destructors
	Char(CHAR Value): TypedVariable(nullptr, CharHelper::ToUnicode(Value)) {}
	Char(WCHAR Value): TypedVariable(nullptr, Value) {}
	Char(Handle<String> Name, CHAR Value): TypedVariable(Name, CharHelper::ToUnicode(Value)) {}
	Char(Handle<String> Name, WCHAR Value): TypedVariable(Name, Value) {}
};
