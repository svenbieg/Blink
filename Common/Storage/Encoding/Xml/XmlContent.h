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
	static inline Handle<XmlContent> Create(XmlNode* Parent, Handle<String> Value)
		{
		return new XmlContent(Parent, Value);
		}

	// Common
	DynamicHandle<XmlContent, String> Value;
	SIZE_T WriteToStream(OutputStream* Stream, UINT Level)override;

private:
	// Con-/Destructors
	XmlContent(XmlNode* Parent, Handle<String> Value);

	// Common
	VOID OnValueChanged(Handle<String> Value);
};

}}}