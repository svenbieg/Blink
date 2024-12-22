//================
// XmlContent.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamWriter.h"
#include "XmlContent.h"
#include "XmlNode.h"

using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {
		namespace Xml {


//==================
// Con-/Destructors
//==================

XmlContent::XmlContent(XmlNode* parent, Handle<String> value):
XmlElement(parent),
Value(this, value)
{
Value.Changed.Add(this, &XmlContent::OnValueChanged);
}


//========
// Common
//========

SIZE_T XmlContent::WriteToStream(OutputStream* stream, UINT level)
{
StreamWriter writer(stream);
return writer.Print(Value);
}


//================
// Common Private
//================

VOID XmlContent::OnValueChanged(Handle<String> value)
{
if(!Parent)
	return;
auto siblings=Parent->Elements;
if(siblings->GetCount()==1)
	Parent->Value.Set(value, false);
}

}}}