//=====================
// UnwindException.cpp
//=====================

#include "UnwindException.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/System.h"
#include "BitHelper.h"
#include "FlagHelper.h"
#include <assert.h>

using namespace Concurrency;
using namespace Devices::System;
using namespace Runtime;
using namespace Storage::Encoding;

extern "C"
{
BYTE __exidx_start;
BYTE __exidx_end;
VOID exc_restore_context(EXC_FRAME* Context);
VOID exc_resume(VOID* Resume, VOID* Argument);
VOID exc_save_context(EXC_FRAME* Context);
}


//===========
// Namespace
//===========

namespace Runtime {


//========
// Prel31
//========

class Prel31
{
public:
	// Common
	static inline UINT Read(UINT* Value)
		{
		return (SIZE_T)Value+SignExtend(*Value);
		}
	static inline UINT SignExtend(UINT Value)
		{
		return Value|((Value&0x40000000)<<1);
		}
};


//=======
// Index
//=======

const SIZE_T EXIDX_COMPACT=1<<31;
const BITS32 EXIDX_CHOICE={ 0xF, 24 };
const BITS32 EXIDX_EXTRA_WORDS={ 0xFF, 16 };
const SIZE_T EXIDX_INVALID=1;


//==========
// Compiler
//==========

extern "C" VOID __aeabi_atexit()
{
}

extern "C" UINT __aeabi_unwind_cpp_pr0(UINT flags, UnwindException* exc, UnwindContext* context)
{
throw NotImplementedException();
}

extern "C" UINT __aeabi_unwind_cpp_pr1(UINT flags, UnwindException* exc, UnwindContext* context)
{
throw NotImplementedException();
return 0;
}

extern "C" VOID __cxa_end_cleanup()
{
UnwindException::EndCleanup();
}

extern "C" [[noreturn]] VOID _Unwind_Resume(UnwindException* exc)
{
exc->Raise();
}


//==================
// Con-/Destructors
//==================

UnwindException::UnwindException(TypeInfo const* type, Destructor destr):
m_Context({ 0 }),
m_Destructor(destr),
m_Thrown((VOID*)((SIZE_T)this+sizeof(UnwindException))),
m_Type(type)
{
auto task=Task::Get();
if(task->m_Exception)
	delete task->m_Exception;
task->m_Exception=this;
}

UnwindException::~UnwindException()
{
if(m_Destructor)
	m_Destructor(m_Thrown);
}


//========
// Common
//========

const UINT THUMB_BIT=1;

[[noreturn]] VOID UnwindException::Catch(SIZE_T landing_pad, UINT type_id, TypeInfo const* type, VOID* thrown)
{
m_Thrown=thrown;
m_Type=type;
Frame.R0=(SIZE_T)thrown;
Frame.R1=type_id;
Frame.PC=landing_pad|THUMB_BIT;
exc_restore_context(&Frame);
System::Restart();
}

[[noreturn]] VOID UnwindException::Cleanup(SIZE_T landing_pad)
{
Frame.PC=landing_pad|THUMB_BIT;
exc_restore_context(&Frame);
System::Restart();
}

[[noreturn]] VOID UnwindException::EndCleanup()
{
auto task=Task::Get();
auto exc=task->m_Exception;
exc->Frame.SP=exc->m_Context.StackPointer;
exc_resume((VOID*)_Unwind_Resume, exc);
System::Restart();
}

VOID* UnwindException::GetThrownException(TypeInfo const** type)
{
if(type)
	*type=m_Type;
return m_Thrown;
}

[[noreturn]] VOID UnwindException::Raise()
{
while(1)
	{
	FlagHelper::Clear(Frame.PC, THUMB_BIT);
	SIZE_T pc=Frame.PC;
	GetContext(pc-1, &m_Context);
	ParseInstructions();
	if(Frame.PC==pc)
		Frame.PC=Frame.LR;
	if(m_Context.Personality)
		{
		auto func=(Personality)m_Context.Personality;
		func(0, this, &m_Context);
		}
	Frame.SP=m_Context.StackPointer;
	}
}


//================
// Common Private
//================

VOID UnwindException::GetContext(SIZE_T pc, UnwindContext* context)
{
context->InstructionCount=0;
context->Personality=0;
context->ProgramCounter=pc;
UINT exidx_start=(UINT)&__exidx_start;
UINT exidx_end=(UINT)&__exidx_end;
UINT exidx_size=exidx_end-exidx_start;
UINT entry_size=8;
UINT entry_count=exidx_size/entry_size;
UINT entry_first=0;
UINT entry_last=entry_count;
while(entry_first<entry_last)
	{
	UINT entry_id=entry_first+(entry_last-entry_first)/2;
	UINT* entry=(UINT*)(exidx_start+entry_id*entry_size);
	UINT frame_start=Prel31::Read(&entry[0]);
	if(frame_start>pc)
		{
		entry_last=entry_id;
		continue;
		}
	if(frame_start<pc)
		{
		entry_first=entry_id+1;
		continue;
		}
	break;
	}
assert(entry_first>0);
UINT entry_id=entry_first-1;
UINT* entry=(UINT*)(exidx_start+entry_id*entry_size);
UINT frame_start=Prel31::Read(&entry[0]);
UINT frame_end=SIZE_MAX;
if(entry_id+1<entry_count)
	{
	UINT next_id=entry_id+1;
	UINT* next=(UINT*)(exidx_start+next_id*entry_size);
	frame_end=Prel31::Read(&next[0]);
	}
context->FrameStart=frame_start;
context->FrameEnd=frame_end;
assert(entry[1]!=EXIDX_INVALID);
if(FlagHelper::Get(entry[1], EXIDX_COMPACT))
	{
	context->Instruction=entry[1]&0x7FFFFFFF;
	context->InstructionCount=1;
	context->Instructions=&context->Instruction;
	return;
	}
UINT table_pos=Prel31::Read(&entry[1]);
UINT* table=(UINT*)table_pos;
UINT extra_words=0;
if(FlagHelper::Get(table[0], EXIDX_COMPACT))
	{
	throw NotImplementedException();
	}
else
	{
	context->Personality=Prel31::Read(&table[0]);
	extra_words=(table[1]>>24)+1;
	context->LanguageData=table_pos+(extra_words+1)*4;
	}
if(extra_words)
	{
	context->InstructionCount=extra_words;
	context->Instructions=&table[1];
	}
}

VOID UnwindException::ParseInstruction(BYTE const* instr)
{
for(UINT pos=0; pos<sizeof(UINT); pos++)
	{
	BYTE op=instr[pos];
	if(op==0)
		continue;
	if(op<0x40)
		{
		Frame.SP+=(op+1)*4;
		continue;
		}
	if(op<0x80)
		{
		Frame.SP-=(op-0x3F)*4;
		continue;
		}
	if(op<0x90)
		{
		pos++;
		assert(pos<sizeof(UINT));
		UINT mask=TypeHelper::MakeLong(instr[pos], op&0xF)<<4;
		assert(mask!=0);
		auto stack=(UINT*)Frame.SP;
		for(UINT reg=4; reg<EXC_REG_COUNT; reg++)
			{
			if(BitHelper::Get(mask, 1U<<reg))
				Registers[reg]=*stack++;
			}
		if(!BitHelper::Get(mask, 1U<<EXC_REG_STACK))
			Frame.SP=(SIZE_T)stack;
		continue;
		}
	if(op<0xA0)
		{
		UINT reg=4+(op&0xF);
		Frame.SP=Registers[reg];
		continue;
		}
	if(op<0xB0)
		{
		auto stack=(UINT*)Frame.SP;
		UINT count=(op&0x7)+1;
		for(UINT u=0; u<count; u++)
			Registers[4+u]=*stack++;
		if(BitHelper::Get(op, 0x08))
			Frame.LR=*stack++;
		Frame.SP=(SIZE_T)stack;
		break;
		}
	if(op==0xB0)
		break;
	if(op==0xB1)
		{
		pos++;
		assert(pos<sizeof(UINT));
		auto stack=(UINT*)Frame.SP;
		BYTE mask=instr[pos];
		for(UINT reg=0; reg<4; reg++)
			{
			if(BitHelper::Get(mask, 1U<<reg))
				Registers[reg]=*stack++;
			}
		Frame.SP=(SIZE_T)stack;
		continue;
		}
	throw NotImplementedException();
	}
}

VOID UnwindException::ParseInstructions()
{
UINT instr_count=m_Context.InstructionCount;
if(!instr_count)
	return;
assert(instr_count==1);
SIZE_T stack=Frame.SP;
for(UINT pos=0; pos<instr_count; pos++)
	{
	UINT op_codes=TypeHelper::BigEndian(m_Context.Instructions[pos]);
	ParseInstruction((BYTE const*)&op_codes);
	}
m_Context.StackPointer=Frame.SP;
Frame.SP=stack;
}

}