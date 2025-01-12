//============
// unwind.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <cxxabi.h>
#include <unwind.h>
#include "Concurrency/Task.h"
#include "Devices/System/System.h"
#include "Storage/Encoding/Dwarf.h"

using namespace Concurrency;
using namespace Devices::System;
using namespace Storage::Encoding;

extern BYTE __eh_frame_hdr_start;


//============
// Operations
//============

typedef enum
{
OP_NOP=0,
OP_SET_LOC=1,
OP_ADVANCE_LOC1=2,
OP_ADVANCE_LOC2=3,
OP_ADVANCE_LOC4=4,
OP_OFFSET_EXTENDED=5,
OP_RESTORE_EXTENDED=6,
OP_UNDEFINED=7,
OP_SAME_VALUE=8,
OP_REGISTER=9,
OP_REMEMBER_STATE=10,
OP_RESTORE_STATE=11,
OP_DEF_CFA=12,
OP_DEF_CFA_REGISTER=13,
OP_DEF_CFA_OFFSET=14,
OP_DEF_CFA_EXPRESSION=15,
OP_EXPRESSION=16,
OP_OFFSET_EXTENDED_SF=17,
OP_DEF_CFA_SF=18,
OP_DEF_CFA_OFFSET_SF=19,
OP_VAL_OFFSET=20,
OP_VAL_OFFSET_SF=21,
OP_VAL_EXPRESSION=22,
OP_AARCH64_NEGATE_RA_STATE=45,
OP_GNU_ARGS_SIZE=46,
OP_GNU_NEGATIVE_OFFSET_EXTENDED=47,
OP_ADVANCE_LOC=64,
OP_OFFSET=128,
OP_RESTORE=192,
}UNWIND_OP;


//==========
// Compiler
//==========

extern "C" [[noreturn]] VOID _Unwind_Raise(UnwindException* exc)
{
exc->Raise();
}

extern "C" [[noreturn]] VOID _Unwind_Resume(UnwindException* exc)
{
exc->Resume();
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

VOID UnwindException::Catch(SIZE_T landing_pad, UINT type_id, TypeInfo const* type, VOID* thrown)
{
m_Thrown=thrown;
m_Type=type;
Registers[0]=(SIZE_T)thrown;
Registers[1]=type_id;
Registers[EXC_REG_RETURN]=landing_pad;
exc_restore_context(&Frame);
System::Restart();
}

VOID UnwindException::Cleanup(SIZE_T landing_pad)
{
exc_resume(&Frame, landing_pad, this);
System::Restart();
}

VOID* UnwindException::GetThrownException(TypeInfo const** type)
{
if(type)
	*type=m_Type;
return m_Thrown;
}

VOID UnwindException::Raise()noexcept
{
SIZE_T instr_ptr=Registers[EXC_REG_RETURN];
GetContext(instr_ptr, &m_Context);
SIZE_T code_offset=m_Context.InstructionPointer-m_Context.FrameStart;
ParseInstructions(m_Context.CommonInstructions, m_Context.CommonInstructionsLength, -1, &m_Context);
ParseInstructions(m_Context.FrameInstructions, m_Context.FrameInstructionsLength, code_offset, &m_Context);
if(m_Context.Personality)
	{
	auto func=(__gxx_personality_func_t)m_Context.Personality;
	func(0, 0, 0, this, &m_Context);
	}

Registers[EXC_REG_STACK]+=m_Context.StackOffset;
exc_resume(&Frame, (SIZE_T)_Unwind_Raise, this);
System::Restart();
}

VOID UnwindException::Resume()noexcept
{
Registers[EXC_REG_STACK]+=m_Context.StackOffset;
exc_resume(&Frame, (SIZE_T)_Unwind_Raise, this);
System::Restart();
}


//================
// Common Private
//================

VOID UnwindException::GetContext(SIZE_T instr_ptr, UnwindContext* context)
{
context->InstructionPointer=instr_ptr;
SIZE_T eh_frame_hdr=(SIZE_T)&__eh_frame_hdr_start;
Dwarf header(eh_frame_hdr);
BYTE version=header.ReadByte();
assert(version==1);
BYTE eh_frame_ptr_enc=header.ReadByte();
BYTE entry_count_enc=header.ReadByte();
BYTE table_enc=header.ReadByte();
SIZE_T eh_frame=header.ReadEncoded(eh_frame_ptr_enc);
UINT entry_count=0;
if(entry_count_enc!=DW_OMIT)
	entry_count=(UINT)header.ReadEncoded(entry_count_enc);
assert(entry_count>0);
auto table_pos=header.GetPosition();
UINT entry_size=header.GetEncodedSize(table_enc)*2;
UINT entry_first=0;
UINT entry_last=entry_count;
while(entry_first<entry_last)
	{
	UINT entry_id=entry_first+(entry_last-entry_first)/2;
	Dwarf entry(table_pos+entry_id*entry_size);
	SIZE_T FrameStart=entry.ReadEncoded(table_enc, eh_frame_hdr);
	if(FrameStart>instr_ptr)
		{
		entry_last=entry_id;
		continue;
		}
	if(FrameStart<instr_ptr)
		{
		entry_first=entry_id+1;
		continue;
		}
	break;
	}
assert(entry_first>0);
UINT entry_id=entry_first-1;
Dwarf entry(table_pos+entry_id*entry_size);
SIZE_T FrameStart=entry.ReadEncoded(table_enc, eh_frame_hdr);
SIZE_T fde=entry.ReadEncoded(table_enc, eh_frame_hdr);
ParseFrameDescriptor(fde, context);
assert(context->FrameStart==FrameStart);
}

VOID UnwindException::ParseCommonInformation(SIZE_T cie_pos, UnwindContext* context)
{
Dwarf cie(cie_pos);
UINT cie_len=cie.ReadValue<UINT>();
assert(cie_len>0&&cie_len<UINT_MAX);
SIZE_T cie_end=cie.GetPosition()+cie_len;
UINT cie_id=cie.ReadValue<UINT>();
assert(cie_id==0);
BYTE cie_ver=cie.ReadByte();
assert(cie_ver==1||cie_ver==3);
auto str_args=cie.Begin();
assert(str_args[0]=='z');
while(cie.ReadByte());
context->CodeAlign=(UINT)cie.ReadUnsigned();
context->DataAlign=(INT)cie.ReadSigned();
SIZE_T return_reg=(cie_ver==1? cie.ReadByte(): cie.ReadUnsigned());
assert(return_reg==EXC_REG_RETURN);
cie.ReadUnsigned();
context->LanguageEncoding=DW_OMIT;
CHAR c=*str_args++;
while(c)
	{
	switch(c)
		{
		case 'L':
			{
			context->LanguageEncoding=cie.ReadByte();
			assert(context->LanguageEncoding!=DW_OMIT);
			break;
			}
		case 'P':
			{
			BYTE pers_enc=cie.ReadByte();
			context->Personality=cie.ReadEncoded(pers_enc);
			break;
			}
		case 'R':
			{
			context->PointerEncoding=cie.ReadByte();
			break;
			}
		case 'S':
			{
			context->IsSignalFrame=true;
			break;
			}
		case 'z':
			{
			context->HasAugmentation=true;
			break;
			}
		default:
			throw NotImplementedException();
		}
	c=*str_args++;
	}
SIZE_T cie_instr=cie.GetPosition();
context->CommonInstructions=cie_instr;
context->CommonInstructionsLength=cie_end-cie_instr;
}

VOID UnwindException::ParseFrameDescriptor(SIZE_T fde_pos, UnwindContext* context)
{
Dwarf fde(fde_pos);
SIZE_T fde_len=fde.ReadValue<UINT>();
if(fde_len==UINT_MAX)
	fde_len=fde.ReadValue<UINT64>();
assert(fde_len>0);
SIZE_T fde_end=fde.GetPosition()+fde_len;
auto cie_pos=fde.GetPosition();
UINT cie_offset=fde.ReadValue<UINT>();
assert(cie_offset!=0);
cie_pos-=cie_offset;
ParseCommonInformation(cie_pos, context);
SIZE_T frame_start=fde.ReadEncoded(context->PointerEncoding);
SIZE_T frame_size=fde.ReadEncoded(context->PointerEncoding&0xF);
SIZE_T frame_end=frame_start+frame_size;
context->FrameStart=frame_start;
context->FrameEnd=frame_end;
if(context->HasAugmentation)
	{
	SIZE_T args_len=fde.ReadUnsigned();
	SIZE_T args_end=fde.GetPosition()+args_len;
	if(context->LanguageEncoding!=DW_OMIT)
		{
		context->LanguageData=fde.GetPosition();
		auto temp=fde;
		if(temp.ReadEncoded(context->LanguageEncoding&0xF)!=0)
			context->LanguageData=fde.ReadEncoded(context->LanguageEncoding);
		}
	fde.SetPosition(args_end);
	}
SIZE_T fde_instr=fde.GetPosition();
context->FrameInstructions=fde_instr;
context->FrameInstructionsLength=fde_end-fde_instr;
}

VOID UnwindException::ParseInstructions(SIZE_T instr_pos, UINT len, SIZE_T code_offset, UnwindContext* context)
{
Dwarf instr(instr_pos);
SIZE_T instr_end=instr_pos+len;
SIZE_T pc=0;
while(instr.GetPosition()<instr_end)
	{
	if(pc>=code_offset)
		break;
	BYTE op_code=instr.ReadByte();
	BYTE high_op=op_code&0xC0;
	if(high_op)
		{
		BYTE operand=op_code&0x3F;
		switch(high_op)
			{
			case OP_ADVANCE_LOC:
				{
				pc+=operand*context->CodeAlign;
				break;
				}
			case OP_OFFSET:
				{
				BYTE reg=operand;
				INT offset=(INT)instr.ReadUnsigned()*context->DataAlign;
				SetRegister(context, reg, offset);
				break;
				}
			case OP_RESTORE:
				{
				BYTE reg=operand;
				assert(reg<=EXC_REG_COUNT);
				Registers[reg]=context->Registers[reg];
				break;
				}
			}
		continue;
		}
	switch(op_code)
		{
		case OP_ADVANCE_LOC1:
			{
			pc+=(UINT)instr.ReadValue<BYTE>()*context->CodeAlign;
			break;
			}
		case OP_DEF_CFA:
			{
			UINT reg=(UINT)instr.ReadUnsigned();
			assert(reg==EXC_REG_STACK);
			context->StackOffset=(INT)instr.ReadUnsigned();
			break;
			}
		case OP_DEF_CFA_OFFSET:
			{
			context->StackOffset=(INT)instr.ReadUnsigned();
			break;
			}
		case OP_DEF_CFA_REGISTER:
			{
			UINT reg=(UINT)instr.ReadUnsigned();
			assert(reg==EXC_REG_STACK);
			break;
			}
		case OP_NOP:
			break;
		case OP_VAL_OFFSET:
			{
			UINT reg=(UINT)instr.ReadUnsigned();
			INT offset=(INT)instr.ReadUnsigned()*context->DataAlign;
			SetRegister(context, reg, offset);
			break;
			}
		default:
			throw NotImplementedException();
		}
	}
}

VOID UnwindException::SetRegister(UnwindContext* context, UINT reg, INT offset)
{
assert(reg<=EXC_REG_COUNT);
context->Registers[reg]=Registers[reg];
SIZE_T stack=Frame.SP+context->StackOffset;
Registers[reg]=*(SIZE_T*)(stack+offset);
}
