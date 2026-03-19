//=====================
// VariableArguments.h
//=====================

#pragma once


//=======
// Using
//=======

#include "BitHelper.h"
#include "CharHelper.h"
#include <bit>


//======================
// Forward-Declarations
//======================

class String;
template <class _obj_t> class Handle;


//===================
// Variable Argument
//===================

class VariableArgument
{
public:
	// Con-/Destructors
	VariableArgument(VariableArgument const&)=delete;
	VariableArgument(Handle<String> Handle)noexcept;
	inline VariableArgument(BYTE Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(BYTE)), m_Value(Value) {}
	inline VariableArgument(WORD Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(WORD)), m_Value(Value) {}
	inline VariableArgument(UINT Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(UINT)), m_Value(Value) {}
	inline VariableArgument(UINT64 Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(UINT64)), m_Value(Value) {}
	inline VariableArgument(SHORT Value)noexcept: m_Flags(VAF_INT), m_Size(sizeof(SHORT)), m_Value(Value) {}
	inline VariableArgument(INT Value)noexcept: m_Flags(VAF_INT), m_Size(sizeof(INT)), m_Value(Value) {}
	inline VariableArgument(INT64 Value)noexcept: m_Flags(VAF_INT), m_Size(sizeof(INT64)), m_Value(Value) {}
	inline VariableArgument(FLOAT Value)noexcept: m_Flags(VAF_FLOAT), m_Size(sizeof(FLOAT)), m_Value(std::bit_cast<UINT64, DOUBLE>(Value)) {}
	inline VariableArgument(DOUBLE Value)noexcept: m_Flags(VAF_FLOAT), m_Size(sizeof(DOUBLE)), m_Value(std::bit_cast<UINT64, DOUBLE>(Value)) {}
	inline VariableArgument(CHAR Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(CHAR)), m_Value(Value) {}
	inline VariableArgument(WCHAR Value)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT), m_Size(sizeof(WCHAR)), m_Value(Value) {}
	inline VariableArgument(LPSTR Pointer)noexcept: m_Flags(VAF_CHAR|VAF_PTR), m_Size(sizeof(CHAR)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(LPCSTR Pointer)noexcept: m_Flags(VAF_CHAR|VAF_PTR|VAF_CONST), m_Size(sizeof(CHAR)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(LPWSTR Pointer)noexcept: m_Flags(VAF_CHAR|VAF_PTR), m_Size(sizeof(WCHAR)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(LPCWSTR Pointer)noexcept: m_Flags(VAF_CHAR|VAF_PTR|VAF_CONST), m_Size(sizeof(WCHAR)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(BYTE* Pointer)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT|VAF_PTR), m_Size(sizeof(BYTE)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(WORD* Pointer)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT|VAF_PTR), m_Size(sizeof(WORD)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(UINT* Pointer)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT|VAF_PTR), m_Size(sizeof(UINT)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(UINT64* Pointer)noexcept: m_Flags(VAF_UNSIGNED|VAF_INT|VAF_PTR), m_Size(sizeof(UINT64)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(SHORT* Pointer)noexcept: m_Flags(VAF_INT|VAF_PTR), m_Size(sizeof(SHORT)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(INT* Pointer)noexcept: m_Flags(VAF_INT|VAF_PTR), m_Size(sizeof(INT)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(INT64* Pointer)noexcept: m_Flags(VAF_INT|VAF_PTR), m_Size(sizeof(INT64)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(FLOAT* Pointer)noexcept: m_Flags(VAF_FLOAT|VAF_PTR), m_Size(sizeof(FLOAT)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(DOUBLE* Pointer)noexcept: m_Flags(VAF_FLOAT|VAF_PTR), m_Size(sizeof(DOUBLE)), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline VariableArgument(Handle<String>* Handle)noexcept: m_Flags(VAF_CHAR|VAF_PTR|VAF_STRING), m_Size(sizeof(TCHAR)), m_Value(reinterpret_cast<UINT64>(Handle)) {}

	// Access
	template <std::integral _int_t> BOOL Get(_int_t& Value)const noexcept
		{
		if(BitHelper::Get(m_Flags, VAF_PTR))
			return false;
		if(BitHelper::Get(m_Flags, VAF_FLOAT))
			{
			auto value=std::bit_cast<DOUBLE, UINT64>(m_Value);
			return TypeHelper::Integral<_int_t, DOUBLE>(value, &Value);
			}
		return TypeHelper::Integral<_int_t, UINT64>(m_Value, &Value);
		}
	template <std::floating_point _float_t> BOOL Get(_float_t& Value)const noexcept
		{
		if(BitHelper::Get(m_Flags, VAF_PTR))
			return false;
		if(BitHelper::Get(m_Flags, VAF_FLOAT))
			{
			auto value=std::bit_cast<DOUBLE, UINT64>(m_Value);
			return TypeHelper::Float<_float_t, DOUBLE>(value, &Value);
			}
		return TypeHelper::Float<_float_t, UINT64>(m_Value, &Value);
		}
	inline BOOL Get(LPSTR& Pointer)const noexcept { return GetStringPointer(VAF_PTR, sizeof(CHAR), (VOID**)&Pointer); }
	inline BOOL Get(LPCSTR& Pointer)const noexcept { return GetStringPointer(VAF_PTR|VAF_CONST, sizeof(CHAR), (VOID**)&Pointer); }
	inline BOOL Get(LPWSTR& Pointer)const noexcept { return GetStringPointer(VAF_PTR, sizeof(WCHAR), (VOID**)&Pointer); }
	inline BOOL Get(LPCWSTR& Pointer)const noexcept { return GetStringPointer(VAF_PTR|VAF_CONST, sizeof(WCHAR), (VOID**)&Pointer); }
	BOOL Get(Handle<String>*& Pointer)const noexcept;

	// Modification
	template <std::numeric _num_t> BOOL Set(_num_t Value)noexcept
		{
		if(!BitHelper::Get(m_Flags, VAF_PTR))
			return false;
		if(BitHelper::Get(m_Flags, VAF_UNSIGNED))
			return SetUnsigned(Value);
		if(BitHelper::Get(m_Flags, VAF_INT))
			return SetSigned(Value);
		if(BitHelper::Get(m_Flags, VAF_FLOAT))
			return SetFloat(Value);
		return false;
		}

private:
	// Flags
	static const BYTE VAF_UNSIGNED=(1<<0);
	static const BYTE VAF_INT=(1<<1);
	static const BYTE VAF_FLOAT=(1<<2);
	static const BYTE VAF_CHAR=(1<<3);
	static const BYTE VAF_PTR=(1<<4);
	static const BYTE VAF_CONST=(1<<5);
	static const BYTE VAF_STRING=(1<<6);

	// Access
	BOOL GetStringPointer(BYTE Flags, BYTE Size, VOID** Pointer)const noexcept;

	// Modification
	template <std::numeric _value_t> BOOL SetFloat(_value_t value)noexcept
		{
		switch(m_Size)
			{
			case sizeof(FLOAT):
				{
				auto ptr=reinterpret_cast<FLOAT*>(m_Value);
				return TypeHelper::Float(value, ptr);
				}
			case sizeof(DOUBLE):
				{
				auto ptr=reinterpret_cast<DOUBLE*>(m_Value);
				return TypeHelper::Float(value, ptr);
				}
			}
		return false;
		}
	template <std::numeric _value_t> BOOL SetSigned(_value_t value)noexcept
		{
		switch(m_Size)
			{
			case sizeof(CHAR):
				{
				auto ptr=reinterpret_cast<CHAR*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(SHORT):
				{
				auto ptr=reinterpret_cast<SHORT*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(INT):
				{
				auto ptr=reinterpret_cast<INT*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(INT64):
				{
				auto ptr=reinterpret_cast<INT64*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			}
		return false;
		}
	template <std::numeric _value_t> BOOL SetUnsigned(_value_t value)noexcept
		{
		switch(m_Size)
			{
			case sizeof(BYTE):
				{
				auto ptr=reinterpret_cast<BYTE*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(WORD):
				{
				auto ptr=reinterpret_cast<WORD*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(UINT):
				{
				auto ptr=reinterpret_cast<UINT*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			case sizeof(UINT64):
				{
				auto ptr=reinterpret_cast<UINT64*>(m_Value);
				return TypeHelper::Integral(value, ptr);
				}
			}
		return false;
		}

	// Common
	BYTE m_Flags;
	BYTE m_Size;
	UINT64 m_Value;
};


//====================
// Variable Arguments
//====================

class VariableArguments
{
public:
	// Con-/Destructors
	VariableArguments(VariableArguments const&)=delete;
	VariableArguments(VariableArgument* Arguments, UINT Count)noexcept: m_Arguments(Arguments), m_Count(Count), m_Position(0) {}

	// Access
	template <class _item_t> BOOL Get(_item_t& Item)noexcept
		{
		if(m_Position>=m_Count)
			return false;
		if(m_Arguments[m_Position].Get(Item))
			{
			m_Position++;
			return true;
			}
		return false;
		}
	inline VOID Reset()noexcept { m_Position=0; }
	template <class _item_t> BOOL Set(_item_t Item)noexcept
		{
		if(m_Position>=m_Count)
			return false;
		if(m_Arguments[m_Position].Set(Item))
			{
			m_Position++;
			return true;
			}
		return false;
		}
	BOOL Set(LPCSTR Buffer, UINT Length)noexcept;
	BOOL Set(LPCWSTR Buffer, UINT Length)noexcept;

private:
	// Common
	VariableArgument* m_Arguments;
	UINT m_Count;
	UINT m_Position;
};
