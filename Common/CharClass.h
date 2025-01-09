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

class Char: public Variable
{
public:
	// Con-/Destructors
	static inline Handle<Char> Create(Handle<String> Name, CHAR Value) { return new Char(Name, Value); }
	static inline Handle<Char> Create(Handle<String> Name, WCHAR Value) { return new Char(Name, Value); }

	// Access
	inline WCHAR Get()
		{
		WCHAR value=m_Value;
		Reading(this, value);
		return value;
		}
	static inline BOOL Get(Char* Value) { return Value? Value->Get(): false; }
	inline Handle<String> GetName()const override { return m_Name; }
	inline BOOL IsAlpha() { return CharHelper::IsAlpha(Get()); }
	inline BOOL IsBreak() { return CharHelper::IsBreak(Get()); }
	inline BOOL IsCapital() { return CharHelper::IsCapital(Get()); }
	inline BOOL IsDigit(UINT Base=10) { return CharHelper::IsDigit(Get(), Base); }
	inline BOOL IsSmall() { return CharHelper::IsSmall(Get()); }
	inline BOOL IsSpecial() { return CharHelper::IsSpecial(Get()); }
	Event<Variable, WCHAR&> Reading;
	Handle<String> ToString(LanguageCode Language)override { return String::Create("%c", Get()); }
	SIZE_T WriteToStream(OutputStream* Stream)override
		{
		if(!Stream)
			return sizeof(WCHAR);
		WCHAR value=Get();
		return Stream->Write(&value, sizeof(WCHAR));
		}

	// Modification
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override
		{
		if(!Stream)
			return sizeof(WCHAR);
		WCHAR value;
		SIZE_T size=Stream->Read(&value, sizeof(WCHAR));
		if(size==sizeof(WCHAR))
			Set(value, Notify);
		return size;
		}
	VOID Set(WCHAR Value, BOOL Notify=true)
		{
		if(m_Value==Value)
			return;
		m_Value=Value;
		if(Notify)
			Changed(this);
		}

private:
	// Con-/Destructors
	Char(Handle<String> Name, CHAR Value): m_Name(Name), m_Value(CharHelper::ToUnicode(Value)) {}
	Char(Handle<String> Name, WCHAR Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	WCHAR m_Value;
};
