//===================
// UnwindException.h
//===================

#pragma once


//=======
// Using
//=======

#include "Runtime/ExceptionFrame.h"
#include "TypeInfo.h"


//===========
// Namespace
//===========

namespace Runtime {


//=========
// Context
//=========

typedef struct
{
SIZE_T FrameStart;
SIZE_T FrameEnd;
UINT Instruction;
UINT InstructionCount;
UINT const* Instructions;
SIZE_T LanguageData;
SIZE_T Personality;
SIZE_T ProgramCounter;
SIZE_T StackPointer;
}UnwindContext;


//===========
// Exception
//===========

class UnwindException
{
public:
	// Using
	typedef VOID(*Destructor)(VOID*);
	typedef UINT (*Personality)(UINT Flags, UnwindException* Exception, UnwindContext* Context);

	// Con-/Destructors
	UnwindException(TypeInfo const* Type, Destructor Destructor);
	~UnwindException();

	// Common
	[[noreturn]] VOID Catch(SIZE_T LandingPad, UINT TypeId, TypeInfo const* Type, VOID* Thrown);
	[[noreturn]] VOID Cleanup(SIZE_T LandingPad);
	[[noreturn]] static VOID EndCleanup();
	VOID* GetThrownException(TypeInfo const** Type=nullptr);
	union
		{
		EXC_FRAME Frame;
		SIZE_T Registers[EXC_REG_COUNT];
		};
	[[noreturn]] VOID Raise();

private:
	// Common
	VOID GetContext(SIZE_T Instruction, UnwindContext* Context);
	VOID ParseInstruction(BYTE const* Instruction);
	VOID ParseInstructions();
	UnwindContext m_Context;
	Destructor m_Destructor;
	VOID* m_Thrown;
	TypeInfo const* m_Type;
};

}