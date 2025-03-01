//==============
// TypeInfo.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <cstddef>
#include "FlagHelper.h"
#include "TypeInfo.h"


//===========
// Namespace
//===========

namespace __cxxabiv1 {


//===========
// Base-Type
//===========

class BaseType
{
public:
	// Common
	inline BOOL IsPublic()const { return FlagHelper::Get(m_Offset, OffsetFlags::Public); }
	inline BOOL IsVirtual()const { return FlagHelper::Get(m_Offset, OffsetFlags::Virtual); }
	inline TypeInfo const* GetInfo()const { return m_Info; }
	inline UINT GetOffset()const { return m_Offset>>8; }
	static VOID* Upcast(BaseType const& Type, VOID* Object)
		{
		INT offset=Type.GetOffset();
		if(!Type.IsVirtual())
			return (BYTE*)Object+offset;
		auto vtable=*(LPCSTR*)Object;
		offset=*(INT*)(vtable+offset);
		return (BYTE*)Object+offset;
		}

private:
	// Flags
	enum class OffsetFlags: UINT
		{
		Virtual=1,
		Public=2
		};

	// Common
	TypeInfo const* m_Info;
	INT m_Offset;
};


//=======
// Class
//=======

class __class_type_info: public TypeInfo
{
public:
	// Common
	BOOL TryUpcast(TypeInfo const* Type, VOID** Thrown)const noexcept override
		{
		if(*Type==*this)
			return true;
		return Type->TryUpcast(this, Thrown);
		}
};


//==================
// Single-Inherited
//==================

class __si_class_type_info: public __class_type_info
{
public:
	// Common
	BOOL TryUpcast(TypeInfo const* Type, VOID** Object)const noexcept override;

private:
	__class_type_info const* m_Info;
};

BOOL __si_class_type_info::TryUpcast(TypeInfo const* type, VOID** obj)const noexcept
{
if(*m_Info==*type)
	return true;
return m_Info->TryUpcast(type, obj);
}


//=================
// Multi-Inherited
//=================

class __vmi_class_type_info: public __class_type_info
{
public:
	// Common
	virtual BOOL TryUpcast(TypeInfo const* Type, VOID** Object)const noexcept override;

protected:
	// Common
	UINT m_Flags;
	UINT m_BaseCount;
	BaseType m_BaseType[];
};

BOOL __vmi_class_type_info::TryUpcast(TypeInfo const* type, VOID** obj)const noexcept
{
for(UINT u=0; u<m_BaseCount; u++)
	{
	if(!m_BaseType[u].IsPublic())
		continue;
	VOID* parent=BaseType::Upcast(m_BaseType[u], *obj);
	auto parent_type=m_BaseType[u].GetInfo();
	if(*parent_type==*type)
		{
		*obj=parent;
		return true;
		}
	if(parent_type->TryUpcast(type, obj))
		return true;
	}
return false;
}


//===============
// Virtual Table
//===============

class VTable
{
public:
	// Common
	static inline VOID* GetWholeObject(VOID const* Object, __class_type_info const** Type)
		{
		SIZE_T obj_pos=(SIZE_T)Object;
		auto vtable=Open(Object);
		auto whole_obj=(VOID*)(obj_pos+vtable->m_MostDerivedOffset);
		auto whole_type=vtable->m_MostDerivedType;
		auto whole_vtable=Open(whole_obj);
		assert(whole_vtable->m_MostDerivedType==whole_type);
		*Type=whole_type;
		return whole_obj;
		}

private:
	// Common
	static inline VTable const* Open(VOID const* Object)
		{
		SIZE_T vtable_pos=*(SIZE_T*)Object;
		vtable_pos-=offsetof(VTable, m_Origin);
		return (VTable const*)vtable_pos;
		}
	INT m_MostDerivedOffset;
	__class_type_info const* m_MostDerivedType;
	INT m_Origin;
};


//==============
// Dynamic cast
//==============

enum DYN_CAST_HINT: INT
{
HINT_NONE=-1,
HINT_NO_PUBLIC_BASE=-2,
HINT_NO_VIRTUAL_BASE=-3
};

extern "C" VOID* __dynamic_cast(VOID const* obj, __class_type_info const* src_type, __class_type_info const* dst_type, SIZE_T offset)
{
if(!obj)
	return nullptr;
auto hint=(DYN_CAST_HINT)offset;
assert(hint!=HINT_NO_VIRTUAL_BASE);
if(hint<HINT_NONE)
	return nullptr;
__class_type_info const* whole_type=nullptr;
auto whole_obj=VTable::GetWholeObject(obj, &whole_type);
if(*whole_type==*dst_type)
	return whole_obj;
if(whole_type->TryUpcast(dst_type, &whole_obj))
	return whole_obj;
return nullptr;
}

}