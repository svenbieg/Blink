//===============
// StringClass.h
//===============

#pragma once


//=======
// Using
//=======

#include "Handle.h"
#include "StringHelper.h"


//========
// String
//========

class String: public Object
{
public:
	// Con-/Destructors
	String(): m_Buffer(nullptr) {}
	String(LPCSTR Value): m_Buffer(nullptr) { Create(0, Value); }
	String(LPCWSTR Value): m_Buffer(nullptr) { Create(0, Value); }
	String(UINT Length, nullptr_t): m_Buffer(new TCHAR[Length+1]) {}
	String(UINT Length, LPCSTR Value): m_Buffer(nullptr) { Create(Length, Value); }
	String(UINT Length, LPCWSTR Value): m_Buffer(nullptr) { Create(Length, Value); }
	template <class... _args_t> String(LPCSTR Format, _args_t... Arguments): m_Buffer(nullptr)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		UINT len=StringPrintArgs((LPSTR)nullptr, 0, Format, vargs);
		m_Buffer=new TCHAR[len+1];
		StringPrintArgs(m_Buffer, len+1, Format, vargs);
		}
	~String()
		{
		if(m_Buffer)
			delete m_Buffer;
		}

	// Access
	inline LPCTSTR Begin()const { return m_Buffer; }
	inline BOOL Contains(LPCSTR Value, BOOL CaseSensitive=true)
		{
		return StringFind(m_Buffer, Value, nullptr, CaseSensitive);
		}
	inline BOOL Contains(LPCWSTR Value, BOOL CaseSensitive=true)
		{
		return StringFind(m_Buffer, Value, nullptr, CaseSensitive);
		}
	inline UINT GetLength()const { return StringLength(m_Buffer); }
	inline bool IsEmpty()const { return !StringIsSet(m_Buffer); }
	template <class... _args_t> inline UINT Scan(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		return StringScanArgs(m_Buffer, Format, vargs);
		}
	Handle<String> ToString()override;

	// Comparison
	inline bool operator==(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)==0; }
	inline bool operator!=(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)!=0; }
	inline bool operator>(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)>0; }
	inline bool operator>=(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)>=0; }
	inline bool operator<(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)<0; }
	inline bool operator<=(String const& String)const { return StringCompare(m_Buffer, String.m_Buffer)<=0; }

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);

private:
	// Common
	template <class _char_t> inline VOID Create(UINT copy, _char_t const* value)
		{
		UINT len=StringLength(value, copy);
		m_Buffer=new TCHAR[len+1];
		StringCopy(m_Buffer, len+1, value);
		}
	LPTSTR m_Buffer;
};


//===============
// String-Handle
//===============

template <>
class Handle<String>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): m_Object(nullptr) {}
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(LPCSTR Value) { HandleCreate<String, String>(&m_Object, new String(Value)); }
	Handle(LPCWSTR Value) { HandleCreate<String, String>(&m_Object, new String(Value)); }
	Handle(String* Value) { HandleCreate<String, String>(&m_Object, Value); }
	Handle(Handle<String> const& Handle) { HandleCreate<String, String>(&m_Object, Handle.m_Object); }
	Handle(Handle<String>&& Handle): m_Object(Handle.m_Object) { Handle.m_Object=nullptr; }
	~Handle() { HandleClear(&m_Object); }

	// Access
	inline operator String*()const { return m_Object; }
	inline String* operator->()const { return m_Object; }

	// Comparison
	inline bool operator==(nullptr_t)const { return !(m_Object&&m_Object->GetLength()); }
	inline bool operator==(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)==0;
		}
	inline bool operator==(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)==0;
		}
	inline bool operator==(Handle<String> const& Handle)const
		{
		auto str1=m_Object? m_Object->Begin(): nullptr;
		auto str2=Handle? Handle->Begin(): nullptr;
		return StringCompare(str1, str2)==0;
		}
	inline bool operator!=(nullptr_t)const { return (m_Object&&m_Object->GetLength()); }
	inline bool operator!=(LPCSTR Value)const { return !operator==(Value); }
	inline bool operator!=(LPCWSTR Value)const { return !operator==(Value); }
	inline bool operator!=(Handle<String> const& Handle)const { return !operator==(Handle); }
	inline bool operator>(nullptr_t)const { return (m_Object&&m_Object->GetLength()); }
	inline bool operator>(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)>0;
		}
	inline bool operator>(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)>0;
		}
	inline bool operator>(Handle<String> const& Object)const
		{
		auto str1=m_Object? m_Object->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)>0;
		}
	inline bool operator>=(nullptr_t)const { return true; }
	inline bool operator>=(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)>=0;
		}
	inline bool operator>=(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)>=0;
		}
	inline bool operator>=(Handle<String> const& Object)const
		{
		auto str1=m_Object? m_Object->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)>=0;
		}
	inline bool operator<(nullptr_t)const { return false; }
	inline bool operator<(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)<0;
		}
	inline bool operator<(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)<0;
		}
	inline bool operator<(Handle<String> const& Object)const
		{
		auto str1=m_Object? m_Object->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)<0;
		}
	inline bool operator<=(nullptr_t)const { return !(m_Object&&m_Object->GetLength()); }
	inline bool operator<=(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)<=0;
		}
	inline bool operator<=(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringCompare(str, Value)<=0;
		}
	inline bool operator<=(Handle<String> const& Object)const
		{
		auto str1=m_Object? m_Object->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)<=0;
		}

	// Assignment
	inline Handle& operator=(nullptr_t) { HandleClear(&m_Object); return *this; }
	Handle& operator=(LPCSTR Value)
		{
		if(m_Object)
			{
			if(StringCompare(m_Object->Begin(), Value)==0)
				return *this;
			m_Object->Release();
			m_Object=nullptr;
			}
		if(Value&&Value[0])
			{
			m_Object=new String(Value);
			m_Object->AddReference();
			}
		return *this;
		}
	Handle& operator=(LPCWSTR Value)
		{
		if(m_Object)
			{
			if(StringCompare(m_Object->Begin(), Value)==0)
				return *this;
			m_Object->Release();
			m_Object=nullptr;
			}
		if(Value&&Value[0])
			{
			m_Object=new String(Value);
			m_Object->AddReference();
			}
		return *this;
		}
	inline Handle& operator=(Handle const& Handle)
		{
		HandleAssign(&m_Object, Handle.m_Object);
		return *this;
		}

	// Operators
	inline Handle<String> operator+(LPCSTR Append)
		{
		if(!m_Object)
			return new String(Append);
		return new String("%s%s", m_Object->Begin(), Append);
		}
	inline Handle<String> operator+(LPCWSTR Append)
		{
		if(!m_Object)
			return new String(Append);
		return new String("%s%s", m_Object->Begin(), Append);
		}
	inline Handle<String> operator+(String* Append)
		{
		if(!m_Object)
			return Append;
		if(!Append)
			return m_Object;
		return new String("%s%s", m_Object->Begin(), Append->Begin());
		}

private:
	// Common
	String* m_Object;
};
