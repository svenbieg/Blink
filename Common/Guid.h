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
	// Using
	static constexpr BYTE NULL_VALUE[16]={ 0 };

	// Con-/Destructors
	GLOBAL_UNIQUE_ID() { MemoryHelper::Fill(m_Value, sizeof(m_Value), 0); }
	GLOBAL_UNIQUE_ID(GLOBAL_UNIQUE_ID const& Id) { MemoryHelper::Copy(m_Value, Id.m_Value, 16); }
	GLOBAL_UNIQUE_ID(DWORD Data1, WORD Data2, WORD Data3, QWORD Data4);
	GLOBAL_UNIQUE_ID(DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);

	// Access
	inline operator BOOL() { return m_Value!=NULL_VALUE; }

	// Common
	static BOOL FromString(Handle<String> Value, GLOBAL_UNIQUE_ID* Id);
	inline Handle<String> ToString()const { return ToString(*this); }
	static Handle<String> ToString(GLOBAL_UNIQUE_ID const& Id);

	// Comparison
	inline BOOL operator==(nullptr_t) { return m_Value==NULL_VALUE; }
	inline BOOL operator==(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)==0; }
	inline BOOL operator!=(nullptr_t) { return m_Value!=NULL_VALUE; }
	inline BOOL operator!=(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)!=0; }
	inline BOOL operator>(nullptr_t) { return MemoryHelper::Compare(m_Value, NULL_VALUE, 16)>0; }
	inline BOOL operator>(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)>0; }
	inline BOOL operator>=(nullptr_t) { return true; }
	inline BOOL operator>=(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)>=0; }
	inline BOOL operator<(nullptr_t) { return false; }
	inline BOOL operator<(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)<0; }
	inline BOOL operator<=(nullptr_t) { return MemoryHelper::Compare(m_Value, NULL_VALUE, 16)<=0; }
	inline BOOL operator<=(GLOBAL_UNIQUE_ID const& Id) { return MemoryHelper::Compare(m_Value, Id.m_Value, 16)<=0; }

private:
	// Common
	BYTE m_Value[16];
};


//==========
// Variable
//==========

class Guid: public Variable
{
public:
	// Con-/Destructors
	static Handle<Guid> Create();
	static Handle<Guid> Create(GLOBAL_UNIQUE_ID const& Value);
	static Handle<Guid> Create(DWORD Data1, WORD Data2, WORD Data3, QWORD Data4);
	static Handle<Guid> Create(DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);
	static Handle<Guid> Create(Handle<String> Name);
	static Handle<Guid> Create(Handle<String> Name, GLOBAL_UNIQUE_ID const& Value);
	static Handle<Guid> Create(Handle<String> Name, DWORD Data1, WORD Data2, WORD Data3, BYTE Data4, BYTE Data5, BYTE Data6, BYTE Data7, BYTE Data8, BYTE Data9, BYTE Data10, BYTE Data11);

	// Access
	GLOBAL_UNIQUE_ID Get();
	static inline GLOBAL_UNIQUE_ID Get(Guid* Value) { return Value? Value->Get(): GLOBAL_UNIQUE_ID(); }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, GLOBAL_UNIQUE_ID&> Reading;
	inline Handle<String> ToString(LanguageCode Language=LanguageCode::None)override { return m_Value.ToString(); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> String, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(GLOBAL_UNIQUE_ID const& Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Guid(Handle<String> Name, GLOBAL_UNIQUE_ID const& Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	GLOBAL_UNIQUE_ID m_Value;
};


//========
// Handle
//========

template <>
class Handle<Guid>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): m_Object(nullptr) {}
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(Guid* Object): m_Object(Object)
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	Handle(Handle&& Move)noexcept: m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	~Handle()
		{
		if(m_Object)
			{
			m_Object->Release();
			m_Object=nullptr;
			}
		}

	// Access
	inline operator BOOL()const { return m_Object&&m_Object->Get(); }
	inline operator Guid*()const { return m_Object; }
	inline Guid* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return Guid::Get(m_Object)==nullptr; }
	inline BOOL operator==(Guid* Value)const { return Guid::Get(m_Object)==Guid::Get(Value); }
	inline BOOL operator==(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return Guid::Get(m_Object)!=nullptr; }
	inline BOOL operator!=(Guid* Value)const { return Guid::Get(m_Object)!=Guid::Get(Value); }
	inline BOOL operator!=(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return Guid::Get(m_Object)>nullptr; }
	inline BOOL operator>(Guid* Value)const { return Guid::Get(m_Object)>Guid::Get(Value); }
	inline BOOL operator>(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return true; }
	inline BOOL operator>=(Guid* Value)const { return Guid::Get(m_Object)>=Guid::Get(Value); }
	inline BOOL operator>=(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return false; }
	inline BOOL operator<(Guid* Value)const { return Guid::Get(m_Object)<Guid::Get(Value); }
	inline BOOL operator<(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return Guid::Get(m_Object)<=nullptr; }
	inline BOOL operator<=(Guid* Value)const { return Guid::Get(m_Object)<=Guid::Get(Value); }
	inline BOOL operator<=(GLOBAL_UNIQUE_ID const& Value)const { return Guid::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		this->~Handle();
		return *this;
		}
	Handle& operator=(Guid* Object)
		{
		if(m_Object==Object)
			return *this;
		if(m_Object)
			m_Object->Release();
		m_Object=Object;
		if(m_Object)
			m_Object->m_RefCount++;
		return *this;
		}
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	Handle& operator=(GLOBAL_UNIQUE_ID const& Value)
		{
		if(!m_Object)
			{
			auto value=Guid::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	Guid* m_Object;
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

inline Handle<Guid> Guid::Create(DWORD Data1, WORD Data2, WORD Data3, QWORD Data4)
{
GLOBAL_UNIQUE_ID guid(Data1, Data2, Data3, Data4);
return new Guid(nullptr, guid);
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
