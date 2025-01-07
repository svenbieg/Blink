//============
// Settings.h
//============

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "Storage/Buffer.h"
#include "Variable.h"


//===========
// Namespace
//===========

namespace Collections {


//==========
// Settings
//==========

class Settings: public Object
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;
	using SettingsMap=Collections::map<Handle<String>, Handle<Variable>>;

	// Con-/Destructors
	~Settings();
	static inline Handle<Settings> Create() { return new Settings(); }

	// Common
	BOOL Add(Handle<Variable> Variable);
	Event<Settings> Changed;
	SIZE_T ReadFromStream(InputStream* Stream);
	SIZE_T WriteToStream(OutputStream* Stream);

private:
	// Con-/Destructors
	Settings() {}

	// Common
	VOID OnVariableChanged();
	SIZE_T WriteVariable(OutputStream* Stream, Variable* Variable);
	SettingsMap m_Settings;
};

}