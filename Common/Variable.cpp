//==============
// Variable.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Variable.h"


//=========
// Globals
//=========

Handle<VariableList> Globals;


//============================
// Con-/Destructors Protected
//============================

Variable::Variable(Handle<String> name):
Name(name)
{
if(Name)
	{
	if(!Globals)
		Globals=new VariableList();
	Globals->Add(Name, this);
	}
}

Variable::~Variable()
{
if(Name)
	{
	Globals->Remove(Name);
	if(Globals->GetCount()==0)
		Globals=nullptr;
	}
}
