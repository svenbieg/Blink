//============
// unwind.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <unwind.h>
#include "Devices/System/Cpu.h"
#include "Devices/System/System.h"
#include "Storage/Encoding/Dwarf/Dwarf.h"

using namespace Devices::System;
using namespace Storage::Encoding;

extern BYTE __eh_frame_hdr_start;


//===========
// Namespace
//===========

namespace Runtime {


//==================
// Con-/Destructors
//==================

UnwindException::UnwindException(TypeInfo const* type, Destructor destr):
// Compiler ???
m_ExceptionClass(0),
m_CleanupFunction(nullptr),
m_Private1(0),
m_Private2(0),
// Common
m_Destructor(destr),
m_Registers(),
m_StackOffset(0),
m_StackPosition(0),
m_StackRegister(0),
m_Thrown((VOID*)((SIZE_T)this+sizeof(UnwindException))),
m_Type(type)
{}

UnwindException::~UnwindException()
{
if(m_Destructor)
	m_Destructor(m_Thrown);
}


//========
// Common
//========

VOID* UnwindException::GetThrownException(TypeInfo const** type)
{
*type=m_Type;
return m_Thrown;
}

UnwindStatus UnwindException::InstallContext(SIZE_T instr_ptr, SIZE_T data0, SIZE_T data1)
{
// Todo
throw NotImplementedException();
return UnwindStatus::InstallContext;
}


UnwindStatus UnwindException::InstallHandler(SIZE_T instr_ptr, TypeInfo const* type, VOID* thrown)
{
// Todo
throw NotImplementedException();
return UnwindStatus::HandlerFound;
}

VOID UnwindException::Raise()
{
Cpu::SaveContext((EXC_FRAME*)m_Registers);
SIZE_T instr_ptr=m_Registers[EXC_REG_RETURN];
UnwindContext context={ 0 };
InitializeContext(instr_ptr, &context);
if(Step(&context)!=UnwindStatus::ContinueUnwind)
	System::Reset();
// Todo
throw NotImplementedException();
}


//================
// Common Private
//================

VOID UnwindException::InitializeContext(SIZE_T instr_ptr, UnwindContext* context)
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
if(cie_len==UINT_MAX)
	cie_len=cie.ReadValue<UINT64>();
assert(cie_len>0);
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
assert(return_reg<EXC_REG_COUNT);
context->ReturnRegister=return_reg;
cie.ReadUnsigned();
context->LanguageEncoding=DW_OMIT;
CHAR c=*str_args++;
if(c!='z')
	c=0;
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
	switch(op_code)
		{
		case OP_DEF_CFA:
			{
			m_StackRegister=(UINT)instr.ReadUnsigned();
			m_StackOffset=(INT)instr.ReadUnsigned();
			break;
			}
		case OP_DEF_CFA_EXPRESSION:
			{
			m_StackExpression=(INT)instr.ReadUnsigned();
			break;
			}
		case OP_DEF_CFA_OFFSET:
			{
			m_StackOffset=(INT)instr.ReadUnsigned();
			break;
			}
		case OP_DEF_CFA_REGISTER:
			{
			m_StackRegister=(UINT)instr.ReadUnsigned();
			break;
			}
		case OP_NOP:
			break;
		case OP_VAL_OFFSET:
			{
			UINT reg=(UINT)instr.ReadUnsigned();
			assert(reg<=EXC_REG_COUNT);
			INT offset=(INT)instr.ReadUnsigned()*context->DataAlign;
			SetRegister(context, reg, offset);
			break;
			}
		default:
			{
			BYTE high_op=op_code&0xC0;
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
					assert(reg<=EXC_REG_COUNT);
					INT offset=(INT)instr.ReadUnsigned()*context->DataAlign;
					SetRegister(context, reg, offset);
					break;
					}
				case OP_RESTORE:
					{
					BYTE reg=operand;
					assert(reg<=EXC_REG_COUNT);
					// Todo
					throw NotImplementedException();
					}
				default:
					throw NotImplementedException();
				}
			break;
			}
		}
	}
}

VOID UnwindException::SetRegister(UnwindContext* context, UINT id, INT offset)
{
// Todo
throw NotImplementedException();
}

UnwindStatus UnwindException::Step(UnwindContext* context)
{
SIZE_T code_offset=context->InstructionPointer-context->FrameStart;
ParseInstructions(context->CommonInstructions, context->CommonInstructionsLength, -1, context);
ParseInstructions(context->FrameInstructions, context->FrameInstructionsLength, code_offset, context);
// Todo
throw NotImplementedException();
return UnwindStatus::ContinueUnwind;
}

extern "C" VOID _Unwind_Resume(UnwindException* exc)
{
// Todo
throw NotImplementedException();
}

}