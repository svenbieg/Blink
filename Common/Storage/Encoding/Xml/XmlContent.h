//==============
// XmlContent.h
//==============

#pragma once


//=======
// Using
//=======

#include "XmlElement.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {
		namespace Xml {


//=============
// XML-Content
//=============

class XmlContent: public XmlElement
{
public:
	// Con-/Destructors
	XmlContent(XmlNode* Parent, Handle<String> Value);

	// Common
	DynamicHandle<XmlContent, String> Value;
	SIZE_T WriteToStream(OutputStream* Stream, UINT Level)override;

private:
	// Common
	VOID OnValueChanged(Handle<String> Value);
};

}}}