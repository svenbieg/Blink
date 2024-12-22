//===============
// Application.h
//===============

#pragma once


//===========
// Namespace
//===========

namespace Firmware {


//=============
// Application
//=============

class Application: public Object
{
public:
	// Common
	static Application* Current;
	INT Run();
	VOID Quit();

protected:
	// Con-/Destructors
	Application(LPCSTR Name);

	// Common
	Handle<String> m_Name;
};

}
