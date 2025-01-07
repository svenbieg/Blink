//===========
// XmlNode.h
//===========

#pragma once


//=======
// Using
//=======

#include "Collections/List.h"
#include "Collections/Map.h"
#include "Storage/Streams/StreamReader.h"
#include "XmlElement.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {
		namespace Xml {


//==========
// Xml-Node
//==========

class XmlNode: public XmlElement
{
private:
	// Using
	using AttributeMap=Collections::Map<Handle<String>, Handle<String>>;
	using ElementList=Collections::List<Handle<XmlElement>>;
	using InputStream=Storage::Streams::InputStream;
	using StreamReader=Storage::Streams::StreamReader;

public:
	// Con-/Destructors
	static inline Handle<XmlNode> Create(XmlNode* Parent=nullptr) { return new XmlNode(Parent); }

	// Common
	Handle<AttributeMap> Attributes;
	VOID Clear();
	Handle<ElementList> Elements;
	SIZE_T ReadFromStream(InputStream* Stream);
	Handle<String> Tag;
	DynamicHandle<XmlNode, String> Value;
	SIZE_T WriteToStream(OutputStream* Stream, UINT Level)override;

private:
	// Con-/Destructors
	XmlNode(XmlNode* Parent);

	// Common
	VOID OnValueChanged(Handle<String> Value);
	SIZE_T ReadContent(StreamReader& Reader);
	SIZE_T ReadTag(StreamReader& Reader);
};

}}}