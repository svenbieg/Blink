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
	// Con-/Destructors
	~Application();

	// Common
	static inline Application* Get() { return s_Current; }
	inline LPCTSTR GetName()const { return m_Name; }
	VOID Run();
	VOID Quit();

protected:
	// Con-/Destructors
	Application(LPCTSTR Name);

private:
	// Common
	LPCTSTR m_Name;
	static Application* s_Current;
};

}