//============
// cxxabi.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <cxxabi.h>
#include <new>
#include "Devices/System/System.h"
#include "Storage/Encoding/Dwarf.h"

using namespace Devices::System;
using namespace Storage::Encoding;


//==========
// Settings
//==========

constexpr SIZE_T CATCH_ANY=0x1301FFFF00000000;


//===========
// Namespace
//===========

extern "C" {


//========
// Common
//========

VOID* __cxa_allocate_exception(SIZE_T thrown_size)noexcept
{
SIZE_T size=sizeof(UnwindException)+thrown_size;
auto exc=(UnwindException*)operator new(size, std::nothrow_t());
if(!exc)
	System::Restart();
return (BYTE*)exc+sizeof(UnwindException);
}

VOID __cxa_atexit() {}

VOID* __cxa_begin_catch(VOID* thrown)noexcept
{
return thrown;
}

VOID __cxa_call_terminate(VOID* exc)
{
System::Restart();
}

VOID __cxa_end_catch()noexcept {}

VOID __cxa_end_cleanup()noexcept {}

VOID* __cxa_get_exception_ptr(VOID* thrown)noexcept
{
auto exc=(UnwindException*)((SIZE_T)thrown-sizeof(UnwindException));
return exc->GetThrownException();
}

VOID __cxa_pure_virtual()
{
throw InvalidContextException();
}

VOID __cxa_throw(VOID* thrown, VOID* type, VOID (*destr)(VOID*))
{
auto exc=(UnwindException*)((SIZE_T)thrown-sizeof(UnwindException));
new (exc) UnwindException((TypeInfo const*)type, destr);
exc_save_context(&exc->Frame);
exc->Raise();
System::Restart();
}

VOID __gxx_personality_v0(INT version, UINT flags, UINT64 exc_class, UnwindException* exc, UnwindContext* context)noexcept
{
Dwarf lsda(context->LanguageData);
SIZE_T frame_start=context->FrameStart;
SIZE_T frame_end=context->FrameEnd;
SIZE_T lp_start=lsda.Read();
if(lp_start==0)
	lp_start=frame_start;
BYTE types_enc=lsda.ReadByte();
UINT type_info_len=0;
SIZE_T types_pos=0;
if(types_enc!=DW_OMIT)
	{
	type_info_len=Dwarf::GetEncodedSize(types_enc);
	types_pos=lsda.ReadUnsigned();
	types_pos+=lsda.GetPosition();
	}
BYTE callsite_enc=lsda.ReadByte();
SIZE_T callsite_len=lsda.ReadUnsigned();
SIZE_T callsite_pos=lsda.GetPosition();
SIZE_T callsite_end=callsite_pos+callsite_len;
SIZE_T actions_pos=callsite_end;
SIZE_T instr_offset=context->InstructionPointer-frame_start;
while(lsda.GetPosition()<callsite_end)
	{
	SIZE_T cs_start=lsda.ReadEncoded(callsite_enc);
	SIZE_T cs_len=lsda.ReadEncoded(callsite_enc);
	SIZE_T cs_end=cs_start+cs_len;
	SIZE_T lp_offset=lsda.ReadEncoded(callsite_enc);
	SIZE_T action_id=lsda.ReadUnsigned();
	if(instr_offset<cs_start)
		continue;
	if(instr_offset>cs_end)
		continue;
	if(lp_offset==0)
		return;
	if(action_id==0)
		{
		exc->Cleanup(lp_start+lp_offset);
		System::Restart();
		}
	auto action_pos=actions_pos+(action_id-1);
	while(action_id)
		{
		Dwarf action(action_pos);
		INT64 type_id=action.ReadSigned();
		assert(type_id>=0);
		if(type_id==0)
			return;
		TypeInfo const* thrown_type;
		VOID* thrown=exc->GetThrownException(&thrown_type);
		Dwarf catch_type_entry(types_pos-type_id*type_info_len);
		auto catch_id=catch_type_entry.ReadEncoded(types_enc);
		TypeInfo const* catch_type=thrown_type;
		if(catch_id!=CATCH_ANY)
			catch_type=(TypeInfo const*)catch_id;
		if(catch_type->TryUpcast(thrown_type, &thrown))
			{
			exc->Catch(lp_start+lp_offset, type_id, catch_type, thrown);
			System::Restart();
			}
		auto next_action_offset=action.ReadSigned();
		if(next_action_offset==0)
			break;
		action_pos+=next_action_offset;
		}
	}
}

}