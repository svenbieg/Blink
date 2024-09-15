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
	String(): pBuffer(nullptr) {}
	String(LPCSTR Value): pBuffer(nullptr) { Create(0, Value); }
	String(LPCWSTR Value): pBuffer(nullptr) { Create(0, Value); }
	String(UINT Length, nullptr_t): pBuffer(new TCHAR[Length+1]) {}
	String(UINT Length, LPCSTR Value): pBuffer(nullptr) { Create(Length, Value); }
	String(UINT Length, LPCWSTR Value): pBuffer(nullptr) { Create(Length, Value); }
	template <class... _args_t> String(LPCSTR Format, _args_t... Arguments): pBuffer(nullptr)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		UINT len=StringPrintArgs((LPSTR)nullptr, 0, Format, vargs);
		pBuffer=new TCHAR[len+1];
		StringPrintArgs(pBuffer, len+1, Format, vargs);
		}
	~String()
		{
		if(pBuffer)
			delete pBuffer;
		}

	// Access
	inline LPCTSTR Begin()const { return pBuffer; }
	inline BOOL Contains(LPCSTR Value, BOOL CaseSensitive=true)
		{
		return StringFind(pBuffer, Value, nullptr, CaseSensitive);
		}
	inline BOOL Contains(LPCWSTR Value, BOOL CaseSensitive=true)
		{
		return StringFind(pBuffer, Value, nullptr, CaseSensitive);
		}
	inline UINT GetLength()const { return StringLength(pBuffer); }
	inline bool IsEmpty()const { return !StringIsSet(pBuffer); }
	template <class... _args_t> inline UINT Scan(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		return StringScanArgs(pBuffer, Format, vargs);
		}
	Handle<String> ToString()override;

	// Comparison
	inline bool operator==(String const& String)const { return StringCompare(pBuffer, String.pBuffer)==0; }
	inline bool operator!=(String const& String)const { return StringCompare(pBuffer, String.pBuffer)!=0; }
	inline bool operator>(String const& String)const { return StringCompare(pBuffer, String.pBuffer)>0; }
	inline bool operator>=(String const& String)const { return StringCompare(pBuffer, String.pBuffer)>=0; }
	inline bool operator<(String const& String)const { return StringCompare(pBuffer, String.pBuffer)<0; }
	inline bool operator<=(String const& String)const { return StringCompare(pBuffer, String.pBuffer)<=0; }

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);

private:
	// Common
	template <class _char_t> inline VOID Create(UINT copy, _char_t const* value)
		{
		UINT len=StringLength(value, copy);
		pBuffer=new TCHAR[len+1];
		StringCopy(pBuffer, len+1, value);
		}
	LPTSTR pBuffer;
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
	Handle(): pObject(nullptr) {}
	Handle(nullptr_t): pObject(nullptr) {}
	Handle(LPCSTR Value) { HandleCreate<String, String>(&pObject, new String(Value)); }
	Handle(LPCWSTR Value) { HandleCreate<String, String>(&pObject, new String(Value)); }
	Handle(String* Value) { HandleCreate<String, String>(&pObject, Value); }
	Handle(Handle<String> const& Handle) { HandleCreate<String, String>(&pObject, Handle.pObject); }
	Handle(Handle<String>&& Handle): pObject(Handle.pObject) { Handle.pObject=nullptr; }
	~Handle() { HandleClear(&pObject); }

	// Access
	inline operator String*()const { return pObject; }
	inline String* operator->()const { return pObject; }

	// Comparison
	inline bool operator==(nullptr_t)const { return !(pObject&&pObject->GetLength()); }
	inline bool operator==(LPCSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)==0;
		}
	inline bool operator==(LPCWSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)==0;
		}
	inline bool operator==(Handle<String> const& Handle)const
		{
		auto str1=pObject? pObject->Begin(): nullptr;
		auto str2=Handle? Handle->Begin(): nullptr;
		return StringCompare(str1, str2)==0;
		}
	inline bool operator!=(nullptr_t)const { return (pObject&&pObject->GetLength()); }
	inline bool operator!=(LPCSTR Value)const { return !operator==(Value); }
	inline bool operator!=(LPCWSTR Value)const { return !operator==(Value); }
	inline bool operator!=(Handle<String> const& Handle)const { return !operator==(Handle); }
	inline bool operator>(nullptr_t)const { return (pObject&&pObject->GetLength()); }
	inline bool operator>(LPCSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)>0;
		}
	inline bool operator>(LPCWSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)>0;
		}
	inline bool operator>(Handle<String> const& Object)const
		{
		auto str1=pObject? pObject->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)>0;
		}
	inline bool operator>=(nullptr_t)const { return true; }
	inline bool operator>=(LPCSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)>=0;
		}
	inline bool operator>=(LPCWSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)>=0;
		}
	inline bool operator>=(Handle<String> const& Object)const
		{
		auto str1=pObject? pObject->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)>=0;
		}
	inline bool operator<(nullptr_t)const { return false; }
	inline bool operator<(LPCSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)<0;
		}
	inline bool operator<(LPCWSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)<0;
		}
	inline bool operator<(Handle<String> const& Object)const
		{
		auto str1=pObject? pObject->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)<0;
		}
	inline bool operator<=(nullptr_t)const { return !(pObject&&pObject->GetLength()); }
	inline bool operator<=(LPCSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)<=0;
		}
	inline bool operator<=(LPCWSTR Value)const
		{
		auto str=pObject? pObject->Begin(): nullptr;
		return StringCompare(str, Value)<=0;
		}
	inline bool operator<=(Handle<String> const& Object)const
		{
		auto str1=pObject? pObject->Begin(): nullptr;
		auto str2=Object? Object->Begin(): nullptr;
		return StringCompare(str1, str2)<=0;
		}

	// Assignment
	inline Handle& operator=(nullptr_t) { HandleClear(&pObject); return *this; }
	Handle& operator=(LPCSTR Value)
		{
		if(pObject)
			{
			if(StringCompare(pObject->Begin(), Value)==0)
				return *this;
			pObject->Release();
			pObject=nullptr;
			}
		if(Value&&Value[0])
			{
			pObject=new String(Value);
			pObject->AddReference();
			}
		return *this;
		}
	Handle& operator=(LPCWSTR Value)
		{
		if(pObject)
			{
			if(StringCompare(pObject->Begin(), Value)==0)
				return *this;
			pObject->Release();
			pObject=nullptr;
			}
		if(Value&&Value[0])
			{
			pObject=new String(Value);
			pObject->AddReference();
			}
		return *this;
		}
	inline Handle& operator=(Handle const& Handle)
		{
		HandleAssign(&pObject, Handle.pObject);
		return *this;
		}

	// Operators
	inline Handle<String> operator+(LPCSTR Append)
		{
		if(!pObject)
			return new String(Append);
		return new String("%s%s", pObject->Begin(), Append);
		}
	inline Handle<String> operator+(LPCWSTR Append)
		{
		if(!pObject)
			return new String(Append);
		return new String("%s%s", pObject->Begin(), Append);
		}
	inline Handle<String> operator+(String* Append)
		{
		if(!pObject)
			return Append;
		if(!Append)
			return pObject;
		return new String("%s%s", pObject->Begin(), Append->Begin());
		}

private:
	// Common
	String* pObject;
};
