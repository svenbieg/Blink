//===============
// Application.h
//===============

#pragma once


//===========
// Namespace
//===========

namespace Core {


//=============
// Application
//=============

class Application: public Object
{
public:
	// Common
	static Application* Current;
	LPCSTR Name;
	virtual INT Run();
	virtual VOID Quit();

protected:
	// Con-/Destructors
	Application(LPCSTR Name);
};

}
