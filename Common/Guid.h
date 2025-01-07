//========
// Guid.h
//========

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//========
// Struct
//========

class GLOBAL_UNIQUE_ID
{
public:
	// Con-/Destructors
	GLOBAL_UNIQUE_ID() { MemoryHelper::Fill(m_Value, sizeof(m_Value), 0); }
	GLOBAL_UNIQUE_ID(GLOBAL_UNIQUE_ID const& Id) { MemoryHelper::Copy(m_Value, Id.m_Value, 16); }
	GLOBAL_UNIQUE_ID(DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);

	// Access
	Handle<String> ToString()const { return ToString(*this); }
	static Handle<String> ToString(GLOBAL_UNIQUE_ID const& Id);

	// Comparison
	BOOL operator==(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)==0; }
	BOOL operator>(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)>0; }
	BOOL operator>=(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)>=0; }
	BOOL operator<(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)<0; }
	BOOL operator<=(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)<=0; }

private:
	// Common
	BYTE m_Value[16];
};


//==========
// Variable
//==========

class Guid: public TypedVariable<GLOBAL_UNIQUE_ID>
{
public:
	// Con-/Destructors
	static Handle<Guid> Create();
	static Handle<Guid> Create(GLOBAL_UNIQUE_ID const& Value);
	static Handle<Guid> Create(DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);
	static Handle<Guid> Create(Handle<String> Name);
	static Handle<Guid> Create(Handle<String> Name, GLOBAL_UNIQUE_ID const& Value);
	static Handle<Guid> Create(Handle<String> Name, DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);

	// Common
	Handle<String> ToString(LanguageCode Language)override;

private:
	// Con-/Destructors
	Guid(Handle<String> Name, GLOBAL_UNIQUE_ID const& Value): TypedVariable(Name, Value) {}
};


//=============
// Guid-Handle
//=============

template <>
class Handle<Guid>: public VariableHandle<Guid, GLOBAL_UNIQUE_ID>
{
public:
	// Using
	using _base_t=VariableHandle<Guid, GLOBAL_UNIQUE_ID>;
	using _base_t::_base_t;
};


//==================
// Con-/Destructors
//==================

inline Handle<Guid> Guid::Create()
{
GLOBAL_UNIQUE_ID guid;
return new Guid(nullptr, guid);
}

inline Handle<Guid> Guid::Create(GLOBAL_UNIQUE_ID const& Value)
{
return new Guid(nullptr, Value);
}

inline Handle<Guid> Guid::Create(DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11)
{
GLOBAL_UNIQUE_ID guid(Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11);
return new Guid(nullptr, guid);
}

inline Handle<Guid> Guid::Create(Handle<String> Name)
{
GLOBAL_UNIQUE_ID guid;
return new Guid(Name, guid);
}

inline Handle<Guid> Guid::Create(Handle<String> Name, GLOBAL_UNIQUE_ID const& Value)
{
return new Guid(Name, Value);
}

inline Handle<Guid> Guid::Create(Handle<String> Name, DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11)
{
GLOBAL_UNIQUE_ID guid(Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11);
return new Guid(Name, guid);
}
