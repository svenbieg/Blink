//============
// cxxabi.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <cxxabi.h>
#include <new>
#include "Devices/System/System.h"
#include "Storage/Encoding/Dwarf/Dwarf.h"

using namespace Devices::System;
using namespace Runtime;
using namespace Storage::Encoding;


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
auto exc=(UnwindException*)operator new(size);
if(!exc)
	System::Reset();
return (BYTE*)exc+sizeof(UnwindException);
}

VOID __cxa_atexit() {}

VOID* __cxa_begin_catch(VOID* thrown)
{
return thrown;
}

VOID __cxa_end_catch()
{
}

VOID __cxa_free_exception(VOID* thrown)noexcept
{
auto exc=(UnwindException*)(SIZE_T)thrown-sizeof(UnwindException);
delete(exc);
}

VOID __cxa_throw(VOID* thrown, TypeInfo const* type, VOID (*destr)(VOID*))
{
auto exc=(UnwindException*)(SIZE_T)thrown-sizeof(UnwindException);
new (exc) UnwindException(type, destr);
exc->Raise();
__cxa_begin_catch(thrown);
System::Reset();
}

UnwindStatus __gxx_personality_v0(INT version, UnwindFlags flags, UINT64 exc_class, UnwindException* exc, UnwindContext* context)noexcept
{
TypeInfo const* thrown_type=nullptr;
VOID* thrown=exc->GetThrownException(&thrown_type);
if(GetFlag(flags, UnwindFlags::HandlerFrame))
	return exc->InstallContext(context->InstructionPointer, (SIZE_T)thrown, (SIZE_T)thrown_type);
Dwarf lsda(context->LanguageData);
SIZE_T frame_start=context->FrameStart;
SIZE_T frame_end=context->FrameEnd;
SIZE_T lp_start=lsda.Read();
BYTE types_enc=lsda.ReadByte();
assert(types_enc!=DW_OMIT);
UINT type_info_len=Dwarf::GetEncodedSize(types_enc);
assert(type_info_len>0);
SIZE_T types_offset=lsda.ReadUnsigned();
SIZE_T types_pos=lsda.GetPosition()+types_offset;
BYTE callsite_enc=lsda.ReadByte();
SIZE_T callsite_len=lsda.ReadUnsigned();
SIZE_T callsite_pos=lsda.GetPosition();
SIZE_T actions_pos=callsite_pos+callsite_len;
SIZE_T instr_offset=context->InstructionPointer-frame_start;
while(lsda.GetPosition()<callsite_pos)
	{
	SIZE_T cs_start=lsda.ReadEncoded(callsite_enc);
	assert(instr_offset>=cs_start);
	SIZE_T cs_len=lsda.ReadEncoded(callsite_enc);
	SIZE_T cs_end=cs_start+cs_len;
	SIZE_T lp_offset=lsda.ReadEncoded(callsite_enc);
	SIZE_T action_id=lsda.ReadUnsigned();
	if(instr_offset>=cs_end)
		continue;
	if(lp_offset==0)
		return UnwindStatus::ContinueUnwind;
	if(action_id==0)
		return exc->InstallContext(lp_start+lp_offset, (SIZE_T)exc, 0);
	auto action_pos=actions_pos+(action_id-1);
	while(action_id)
		{
		Dwarf action(action_pos);
		INT64 type_id=action.ReadSigned();
		if(type_id==0)
			{
			if(GetFlag(flags, UnwindFlags::SearchPhase))
				return UnwindStatus::ContinueUnwind;
			return exc->InstallContext(lp_start+lp_offset, (SIZE_T)exc, 0);
			}
		if(type_id>0)
			{
			Dwarf catch_type_entry(types_pos-type_id*type_info_len);
			auto catch_type=(TypeInfo const*)catch_type_entry.ReadEncoded(types_enc);
			if(catch_type==nullptr||catch_type->TryCatch(thrown_type, &thrown))
				return exc->InstallHandler(lp_start+lp_offset, catch_type, thrown);
			}
		else
			{
			Dwarf throw_spec(types_pos-type_id);
			auto ts_index=throw_spec.ReadUnsigned();
			while(ts_index!=0)
				{
				Dwarf catch_type_entry(types_pos-ts_index*type_info_len);
				auto catch_type=(TypeInfo const*)catch_type_entry.ReadEncoded(types_enc);
				if(catch_type->TryCatch(thrown_type, &thrown))
					return exc->InstallHandler(lp_start+lp_offset, catch_type, thrown);
				ts_index=throw_spec.ReadUnsigned();
				}
			}
		auto next_action_offset=action.ReadSigned();
		if(next_action_offset==0)
			break;
		action_pos+=next_action_offset;
		}
	}
return UnwindStatus::ContinueUnwind;
}

}