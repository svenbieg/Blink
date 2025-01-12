//==========
// unwind.h
//==========

#pragma once


//=======
// Using
//=======

#include <except.h>
#include "TypeInfo.h"


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
BOOL IsSignalFrame;
SIZE_T LanguageData;
BYTE LanguageEncoding;
SIZE_T Personality;
BYTE PointerEncoding;
SIZE_T Registers[EXC_REG_COUNT];
INT StackOffset;
}UnwindContext;



//===========
// Exception
//===========

class UnwindException
{
public:
	// Using
	typedef VOID(*Destructor)(VOID*);

	// Con-/Destructors
	UnwindException(TypeInfo const* Type, Destructor Destructor);
	~UnwindException();

	// Common
	[[noreturn]] VOID Catch(SIZE_T LandingPad, UINT TypeId, TypeInfo const* Type, VOID* Thrown);
	[[noreturn]] VOID Cleanup(SIZE_T LandingPad);
	VOID* GetThrownException(TypeInfo const** Type=nullptr);
	union
		{
		EXC_FRAME Frame;
		SIZE_T Registers[EXC_REG_COUNT];
		};
	[[noreturn]] VOID Raise()noexcept;
	[[noreturn]] VOID Resume()noexcept;

private:
	// Common
	VOID GetContext(SIZE_T Instruction, UnwindContext* Context);
	VOID ParseCommonInformation(SIZE_T Position, UnwindContext* Context);
	VOID ParseFrameDescriptor(SIZE_T Position, UnwindContext* Context);
	VOID ParseInstructions(SIZE_T Position, UINT Length, SIZE_T CodeOffset, UnwindContext* Context);
	VOID SetRegister(UnwindContext* Context, UINT Register, INT Offset);
	UnwindContext m_Context;
	Destructor m_Destructor;
	VOID* m_Thrown;
	TypeInfo const* m_Type;
};
