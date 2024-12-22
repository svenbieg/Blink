//==============
// XmlElement.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/OutputStream.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {
		namespace Xml {


//======================
// Forward-Declarations
//======================

class XmlNode;


//=============
// XML-Element
//=============

class XmlElement: public Object
{
public:
	// Using
	using OutputStream=Storage::Streams::OutputStream;

	// Common
	XmlNode* Parent;
	virtual SIZE_T WriteToStream(OutputStream* Stream, UINT Level)=0;

protected:
	// Con-/Destructors
	XmlElement(XmlNode* Parent): Parent(Parent) {}
};

}}}