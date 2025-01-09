//==================
// IntegerClasses.h
//==================

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//=======
// Int32
//=======

class Int32: public Variable
{
public:
	// Con-/Destructors
	static Handle<Int32> Create(INT Value=0);
	static Handle<Int32> Create(Handle<String> Name, INT Value=0);

	// Access
	INT Get();
	static inline INT Get(Int32* Value) { return Value? Value->Get(): 0; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, INT&> Reading;
	inline Handle<String> ToString(LanguageCode Language)override { return String::Create("%i", Get()); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> String, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> String, INT* Value);
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(INT Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Int32(Handle<String> Name, INT Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	INT m_Value;
};


//==============
// Int32-Handle
//==============

template <>
class Handle<Int32>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): m_Object(nullptr) {}
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(Int32* Object): m_Object(Object)
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
	inline operator BOOL()const { return Int32::Get(m_Object)!=0; }
	inline operator Int32*()const { return m_Object; }
	inline Int32* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return Int32::Get(m_Object)==0; }
	inline BOOL operator==(Int32* Value)const { return Int32::Get(m_Object)==Int32::Get(Value); }
	inline BOOL operator==(INT Value)const { return Int32::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return Int32::Get(m_Object)!=0; }
	inline BOOL operator!=(Int32* Value)const { return Int32::Get(m_Object)!=Int32::Get(Value); }
	inline BOOL operator!=(INT Value)const { return Int32::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return Int32::Get(m_Object)>0; }
	inline BOOL operator>(Int32* Value)const { return Int32::Get(m_Object)>Int32::Get(Value); }
	inline BOOL operator>(INT Value)const { return Int32::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return Int32::Get(m_Object)>=0; }
	inline BOOL operator>=(Int32* Value)const { return Int32::Get(m_Object)>=Int32::Get(Value); }
	inline BOOL operator>=(INT Value)const { return Int32::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return Int32::Get(m_Object)<0; }
	inline BOOL operator<(Int32* Value)const { return Int32::Get(m_Object)<Int32::Get(Value); }
	inline BOOL operator<(INT Value)const { return Int32::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return Int32::Get(m_Object)<=0; }
	inline BOOL operator<=(Int32* Value)const { return Int32::Get(m_Object)<=Int32::Get(Value); }
	inline BOOL operator<=(INT Value)const { return Int32::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		this->~Handle();
		return *this;
		}
	Handle& operator=(Int32* Object)
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
	Handle& operator=(INT Value)
		{
		if(!m_Object)
			{
			auto value=Int32::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	Int32* m_Object;
};


//========================
// Int32 Con-/Destructors
//========================

inline Handle<Int32> Int32::Create(INT Value)
{
return new Int32(nullptr, Value);
}

inline Handle<Int32> Int32::Create(Handle<String> Name, INT Value)
{
return new Int32(Name, Value);
}


//=======
// Int64
//=======

class Int64: public Variable
{
public:
	// Con-/Destructors
	static Handle<Int64> Create(INT64 Value=0);
	static Handle<Int64> Create(Handle<String> Name, INT64 Value=0);

	// Access
	INT64 Get();
	static inline INT Get(Int64* Value) { return Value? Value->Get(): 0; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, INT64&> Reading;
	inline Handle<String> ToString(LanguageCode Language)override { return String::Create("%i", Get()); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> String, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> String, INT64* Value);
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(INT64 Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Int64(Handle<String> Name, INT64 Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	INT64 m_Value;
};


//==============
// Int64-Handle
//==============

template <>
class Handle<Int64>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): m_Object(nullptr) {}
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(Int64* Object): m_Object(Object)
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
	inline operator BOOL()const { return Int64::Get(m_Object)!=0; }
	inline operator Int64*()const { return m_Object; }
	inline Int64* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return Int64::Get(m_Object)==0; }
	inline BOOL operator==(Int64* Value)const { return Int64::Get(m_Object)==Int64::Get(Value); }
	inline BOOL operator==(INT64 Value)const { return Int64::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return Int64::Get(m_Object)!=0; }
	inline BOOL operator!=(Int64* Value)const { return Int64::Get(m_Object)!=Int64::Get(Value); }
	inline BOOL operator!=(INT64 Value)const { return Int64::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return Int64::Get(m_Object)>0; }
	inline BOOL operator>(Int64* Value)const { return Int64::Get(m_Object)>Int64::Get(Value); }
	inline BOOL operator>(INT64 Value)const { return Int64::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return Int64::Get(m_Object)>=0; }
	inline BOOL operator>=(Int64* Value)const { return Int64::Get(m_Object)>=Int64::Get(Value); }
	inline BOOL operator>=(INT64 Value)const { return Int64::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return Int64::Get(m_Object)<0; }
	inline BOOL operator<(Int64* Value)const { return Int64::Get(m_Object)<Int64::Get(Value); }
	inline BOOL operator<(INT64 Value)const { return Int64::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return Int64::Get(m_Object)<=0; }
	inline BOOL operator<=(Int64* Value)const { return Int64::Get(m_Object)<=Int64::Get(Value); }
	inline BOOL operator<=(INT64 Value)const { return Int64::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		this->~Handle();
		return *this;
		}
	Handle& operator=(Int64* Object)
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
	Handle& operator=(INT64 Value)
		{
		if(!m_Object)
			{
			auto value=Int64::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	Int64* m_Object;
};


//========================
// Int64 Con-/Destructors
//========================

inline Handle<Int64> Int64::Create(INT64 Value)
{
return new Int64(nullptr, Value);
}

inline Handle<Int64> Int64::Create(Handle<String> Name, INT64 Value)
{
return new Int64(Name, Value);
}
