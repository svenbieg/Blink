//=============
// XmlNode.cpp
//=============

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

XmlNode::XmlNode(XmlNode* parent):
XmlElement(parent),
Value(this)
{
Attributes=new AttributeMap();
Elements=new ElementList();
Value.Changed.Add(this, &XmlNode::OnValueChanged);
}


//========
// Common
//========

VOID XmlNode::Clear()
{
Attributes->Clear();
Elements->Clear();
Tag=nullptr;
Value.Set(nullptr, false);
}

SIZE_T XmlNode::ReadFromStream(InputStream* stream)
{
if(!stream)
	return 0;
StreamReader reader(stream);
SIZE_T size=0;
size+=reader.FindChar("<");
size+=ReadTag(reader);
if(reader.LastChar=='/')
	{
	size+=reader.FindChar(">");
	return size;
	}
size+=ReadContent(reader);
return size;
}

SIZE_T XmlNode::WriteToStream(OutputStream* stream, UINT level)
{
StreamWriter writer(stream);
SIZE_T size=0;
size+=writer.PrintChar('\t', level);
size+=writer.PrintChar('<');
size+=writer.Print(Tag);
for(auto it=Attributes->First(); it->HasCurrent(); it->MoveNext())
	{
	auto name=it->GetKey();
	auto value=it->GetValue();
	size+=writer.Print(" ");
	size+=writer.Print(name);
	if(value)
		{
		size+=writer.Print("=\"");
		size+=writer.Print(value);
		size+=writer.Print("\"");
		}
	}
UINT child_count=Elements->GetCount();
if(child_count==0)
	{
	size+=writer.Print(" />\r\n");
	return size;
	}
size+=writer.Print(">\r\n");
for(auto it=Elements->First(); it->HasCurrent(); it->MoveNext())
	{
	auto child=it->GetCurrent();
	size+=child->WriteToStream(stream, level+1);
	}
size+=writer.PrintChar('\t', level);
size+=writer.Print("</");
size+=writer.Print(Tag);
size+=writer.Print(">\r\n");
return size;
}


//================
// Common Private
//================

VOID XmlNode::OnValueChanged(Handle<String> value)
{
if(Elements->GetCount()==1)
	{
	auto content=Elements->GetAt(0).As<XmlContent>();
	if(content)
		{
		content->Value.Set(value, false);
		return;
		}
	}
Elements->Clear();
Handle<XmlContent> content=new XmlContent(this, value);
Elements->Append(content);
}

SIZE_T XmlNode::ReadContent(StreamReader& reader)
{
SIZE_T size=0;
while(1)
	{
	Handle<String> value=reader.ReadString(&size, "<", " \t\r\n");
	if(value)
		{
		Handle<XmlContent> content=new XmlContent(this, value);
		Elements->Append(content, false);
		continue;
		}
	Handle<XmlNode> child=new XmlNode(this);
	size+=child->ReadTag(reader);
	if(!child->Tag)
		{
		size+=reader.FindChar(">");
		break;
		}
	size+=child->ReadContent(reader);
	Elements->Append(child, false);
	}
if(Elements->GetCount()==1)
	{
	auto content=Elements->GetAt(0).As<XmlContent>();
	if(content)
		Value.Set(content->Value, false);
	}
return size;
}

SIZE_T XmlNode::ReadTag(StreamReader& reader)
{
SIZE_T size=0;
Tag=reader.ReadString(&size, " />");
if(!Tag)
	return size;
while(reader.LastChar==' ')
	{
	Handle<String> att_name=reader.ReadString(&size, " =/>");
	if(att_name)
		{
		Handle<String> att_value;
		if(reader.LastChar=='=')
			{
			att_value=reader.ReadString(&size, "\"", "\"");
			size+=reader.FindChar(" />");
			}
		Attributes->Add(att_name, att_value);
		}
	}
return size;
}

}}}