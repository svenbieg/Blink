//============
// Settings.h
//============

#pragma once


//=======
// Using
//=======

#include "Collections/Map.h"
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

	// Con-/Destructors
	Settings();
	~Settings();

	// Common
	BOOL Add(Handle<Variable> Variable);
	Event<Settings> Changed;
	SIZE_T ReadFromStream(InputStream* Stream);
	SIZE_T WriteToStream(OutputStream* Stream);

private:
	// Using
	using SettingsMap=Collections::Map<Handle<String>, Handle<Variable>>;

	// Common
	VOID OnVariableChanged();
	SIZE_T WriteVariable(OutputStream* Stream, Variable* Variable);
	Handle<SettingsMap> m_SettingsMap;
};

}