//================
// HandleHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "Object.h"


//==================
// Con-/Destructors
//==================

template <class _obj_t> VOID HandleClear(_obj_t** Destination)
{
auto dst=*Destination;
if(dst)
	{
	dst->Release();
	*Destination=nullptr;
	}
}

template <class _obj_t, class _convert_t> VOID HandleCreate(_obj_t** Destination, _convert_t* Source)
{
*Destination=Source;
if(Source)
	Source->AddReference();
}


//============
// Assignment
//============

template <class _obj_t> VOID HandleAssign(_obj_t** Destination, _obj_t* Source)
{
if(Source)
	Source->AddReference();
auto dst=*Destination;
if(dst)
	dst->Release();
*Destination=Source;
}

template <class _obj_t, class _convert_t> VOID HandleAssign(_obj_t** Destination, _convert_t* Source)
{
_obj_t* src=static_cast<_obj_t*>(Source);
if(src)
	src->AddReference();
auto dst=*Destination;
if(dst)
	dst->Release();
*Destination=src;
}
