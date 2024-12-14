//==========
// unwind.h
//==========

#pragma once


//=======
// Using
//=======

#include "Devices/System/Exceptions.h"


//===========
// Namespace
//===========

namespace Runtime {


//========
// Status
//========

enum class UnwindStatus: UINT
{
Success,
ForeignExceptionCaught,
FatalPhase1Error,
FatalPhase2Error,
NormalStop,
EndOfStack,
HandlerFound,
InstallContext,
ContinueUnwind
};


//=======
// Flags
//=======

enum class UnwindFlags: UINT
{
SearchPhase=1,
CleanupPhase=2,
HandlerFrame=4,
ForceUnwind=8,
EndOfStack=8
};


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


//=========
// Context
//=========

typedef struct
{
SIZE_T CommonInstructions;
SIZE_T CommonInstructionsLength;
UINT CodeAlign;
INT DataAlign;
SIZE_T FrameEnd;
SIZE_T FrameInstructions;
SIZE_T FrameInstructionsLength;
SIZE_T FrameStart;
BOOL HasAugmentation;
SIZE_T InstructionPointer;
SIZE_T LanguageData;
BYTE LanguageEncoding;
SIZE_T Personality;
BYTE PointerEncoding;
BYTE ReturnRegister;
BOOL IsSignalFrame;
}UnwindContext;


//===========
// Exception
//===========

class UnwindException
{
public:
	// Using
	typedef VOID(*CleanupFunction)(UnwindStatus, UnwindException*);
	typedef VOID(*Destructor)(VOID*);

	// Con-/Destructors
	UnwindException(TypeInfo const* Type, Destructor Destructor);
	~UnwindException();

	// Common
	VOID* GetThrownException(TypeInfo const** Type);
	UnwindStatus InstallContext(SIZE_T Instruction, SIZE_T Data0, SIZE_T Data1);
	UnwindStatus InstallHandler(SIZE_T Instruction, TypeInfo const* Type, VOID* Thrown);
	VOID Raise();

private:
	// Compiler ???
	UINT64 m_ExceptionClass;
	CleanupFunction m_CleanupFunction;
	SIZE_T m_Private1;
	SIZE_T m_Private2;

	// Common
	VOID InitializeContext(SIZE_T Instruction, UnwindContext* Context);
	VOID ParseCommonInformation(SIZE_T Position, UnwindContext* Context);
	VOID ParseFrameDescriptor(SIZE_T Position, UnwindContext* Context);
	VOID ParseInstructions(SIZE_T Position, UINT Length, SIZE_T CodeOffset, UnwindContext* Context);
	VOID SetRegister(UnwindContext* Context, UINT Register, INT Offset);
	UnwindStatus Step(UnwindContext* Context);
	Destructor m_Destructor;
	SIZE_T m_Registers[Devices::System::EXC_REG_COUNT];
	INT m_StackExpression;
	INT m_StackOffset;
	SIZE_T m_StackPosition;
	UINT m_StackRegister;
	VOID* m_Thrown;
	TypeInfo const* m_Type;
};

}