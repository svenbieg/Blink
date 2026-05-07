//=============
// XmlNode.cpp
//=============

#include "XmlNode.h"


//=======
// Using
//=======

#include "Concurrency/ReadLock.h"
#include "Concurrency/WriteLock.h"
#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"

using namespace Concurrency;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Storage {
	namespace Xml {


//========
// Common
//========

VOID XmlNode::AppendChild(XmlNode* child)
{
WriteLock lock(m_Mutex);
AppendChildInternal(child);
lock.Unlock();
Changed(this);
}

BOOL XmlNode::Clear()
{
WriteLock lock(m_Mutex);
if(!ClearInternal())
	return false;
lock.Unlock();
Changed(this);
return true;
}

VOID XmlNode::CopyFrom(XmlNode* copy)
{
if(!copy)
	throw InvalidArgumentException();
WriteLock lock(m_Mutex);
ReadLock copy_lock(copy->m_Mutex);
BOOL changed=false;
changed|=ClearInternal();
changed|=SetTagInternal(copy->m_Tag);
for(auto const& attr: copy->m_Attributes)
	{
	auto const& key=attr.get_key();
	auto const& value=attr.get_value();
	changed|=SetAttributeInternal(key, value);
	}
if(copy->m_Value)
	{
	changed|=SetValue(copy->m_Value);
	}
else
	{
	changed|=copy->m_Children;
	for(auto const& copy_child: copy->m_Children)
		{
		auto child=XmlNode::Create(this);
		child->CopyFrom(copy_child);
		}
	}
copy_lock.Unlock();
lock.Unlock();
if(changed)
	Changed(this);
}

Handle<String> XmlNode::GetAttribute(Handle<String> key)
{
ReadLock lock(m_Mutex);
return m_Attributes.get(key);
}

BOOL XmlNode::GetAttribute(Handle<String> key, UINT* value_ptr)
{
ReadLock lock(m_Mutex);
Handle<String> value;
if(!m_Attributes.try_get(key, &value))
	return false;
lock.Unlock();
return value->Scan("%u", value_ptr)==1;
}

BOOL XmlNode::GetAttribute(Handle<String> key, UINT64* value_ptr)
{
ReadLock lock(m_Mutex);
Handle<String> value;
if(!m_Attributes.try_get(key, &value))
	return false;
lock.Unlock();
return value->Scan("%u", value_ptr)==1;
}

Handle<XmlNodeAttributeIterator> XmlNode::GetAttributes()
{
return new XmlNodeAttributeIterator(this);
}

Handle<XmlNode> XmlNode::GetChild(Handle<String> name)
{
ReadLock lock(m_Mutex);
return m_Index.get(name);
}

Handle<XmlNode> XmlNode::GetChildAt(UINT pos)
{
ReadLock lock(m_Mutex);
return m_Children.get_at(pos);
}

Handle<XmlNodeChildIterator> XmlNode::GetChildren()
{
return new XmlNodeChildIterator(this);
}

Handle<String> XmlNode::GetName()
{
ReadLock lock(m_Mutex);
Handle<String> name;
if(!m_Attributes.try_get("Name", &name))
	return nullptr;
return name;
}

Handle<String> XmlNode::GetTag()
{
ReadLock lock(m_Mutex);
return m_Tag;
}

Handle<String> XmlNode::GetValue()
{
ReadLock lock(m_Mutex);
return m_Value;
}

BOOL XmlNode::HasAttribute(Handle<String> key)
{
ReadLock lock(m_Mutex);
return m_Attributes.contains(key);
}

VOID XmlNode::InsertChildAt(UINT pos, XmlNode* child)
{
WriteLock lock(m_Mutex);
InsertChildInternal(pos, child);
lock.Unlock();
Changed(this);
}

SIZE_T XmlNode::ReadFromStream(InputStream* stream)
{
StreamReader reader(stream);
SIZE_T read=0;
WriteLock lock(m_Mutex);
auto value=reader.ReadString(&read, "<", "\r\n\t ");
if(value)
	{
	if(m_Tag)
		throw InvalidArgumentException();
	m_Value=value;
	return read;
	}
if(!CharHelper::Equal(reader.LastChar, '<'))
	throw InvalidArgumentException();
auto tag=reader.ReadString(&read, " />", " ");
if(!tag)
	throw InvalidArgumentException();
if(m_Tag)
	{
	if(StringHelper::Compare(m_Tag, tag, 0, false)!=0)
		throw InvalidArgumentException();
	}
while(CharHelper::Equal(reader.LastChar, ' '))
	{
	auto key=reader.ReadString(&read, " =/>", " ");
	if(!key)
		break;
	Handle<String> value;
	if(CharHelper::Equal(reader.LastChar, '='))
		value=reader.ReadString(&read, "\"", " \"");
	m_Attributes.set(key, value);
	}
if(CharHelper::Equal(reader.LastChar, '/'))
	{
	read+=reader.ReadChar();
	if(!CharHelper::Equal(reader.LastChar, '>'))
		throw InvalidArgumentException();
	return read;
	}
while(1)
	{
	auto child=XmlNode::Create();
	read+=child->ReadFromStream(stream);
	if(!child->m_Tag)
		{
		m_Value=child->m_Value;
		break;
		}
	m_Children.append(child);
	child->m_Parent=this;
	auto name=child->GetName();
	if(!name)
		continue;
	if(!m_Index.add(name, child))
		throw InvalidArgumentException();
	}
read+=reader.ReadChar();
if(!CharHelper::Equal(reader.LastChar, '/'))
	throw InvalidArgumentException();
auto close=reader.ReadString(&read, ">");
if(!CharHelper::Equal(reader.LastChar, '>'))
	throw InvalidArgumentException();
if(StringHelper::Compare(close, tag, 0, false)!=0)
	throw InvalidArgumentException();
m_Tag=tag;
return read;
}

BOOL XmlNode::RemoveAttribute(Handle<String> key)
{
WriteLock lock(m_Mutex);
if(!RemoveAttributeInternal(key))
	return false;
lock.Unlock();
Changed(this);
return true;
}

VOID XmlNode::RemoveChildAt(UINT pos)
{
WriteLock lock(m_Mutex);
RemoveChildInternal(pos);
lock.Unlock();
Changed(this);
}

BOOL XmlNode::SetAttribute(Handle<String> key, Handle<String> value)
{
WriteLock lock(m_Mutex);
if(!SetAttributeInternal(key, value))
	return false;
lock.Unlock();
Changed(this);
return true;
}

BOOL XmlNode::SetTag(Handle<String> tag)
{
WriteLock lock(m_Mutex);
if(!SetTagInternal(tag))
	return false;
lock.Unlock();
Changed(this);
return true;
}

BOOL XmlNode::SetValue(Handle<String> value)
{
WriteLock lock(m_Mutex);
if(!SetValueInternal(value))
	return false;
lock.Unlock();
Changed(this);
return true;
}

SIZE_T XmlNode::WriteToStream(OutputStream* stream, INT level)
{
ReadLock lock(m_Mutex);
if(!m_Tag)
	return 0;
StreamWriter writer(stream);
SIZE_T size=0;
INT next_level=-1;
if(level>=0)
	{
	next_level=level+1;
	size+=writer.Print("\r\n");
	}
if(level>0)
	size+=writer.PrintChar('\t', level);
size+=writer.Print("<");
size+=writer.Print(m_Tag);
for(auto const& it: m_Attributes)
	{
	auto name=it.get_key();
	auto value=it.get_value();
	size+=writer.Print(" ");
	size+=writer.Print(name);
	if(value)
		{
		size+=writer.Print("=\"");
		size+=writer.Print(value);
		size+=writer.Print("\"");
		}
	}
if(m_Value)
	{
	size+=writer.Print(">");
	size+=writer.Print(m_Value);
	size+=writer.Print("</");
	size+=writer.Print(m_Tag);
	size+=writer.Print(">");
	return size;
	}
if(!m_Children.get_count())
	{
	size+=writer.Print(" />");
	return size;
	}
size+=writer.Print(">");
for(auto const& child: m_Children)
	size+=child->WriteToStream(stream, next_level);
if(level>=0)
	size+=writer.Print("\r\n");
if(level>0)
	size+=writer.PrintChar('\t', level);
size+=writer.Print("</");
size+=writer.Print(m_Tag);
size+=writer.Print(">");
return size;
}


//============================
// Con-/Destructors Protected
//============================

XmlNode::XmlNode(XmlNode* Parent, Handle<String> tag):
m_Parent(Parent),
m_Tag(tag)
{
if(m_Parent)
	m_Parent->AppendChild(this);
}


//==================
// Common Protected
//==================

VOID XmlNode::AppendChildInternal(XmlNode* child)
{
assert(child);
auto name=child->GetName();
if(name)
	{
	if(m_Index.contains(name))
		throw AlreadyExistsException();
	}
m_Children.append(child);
if(name)
	m_Index.set(name, child);
}

BOOL XmlNode::ClearInternal()
{
bool cleared=false;
cleared|=m_Attributes.clear();
cleared|=m_Children.clear();
cleared|=m_Index.clear();
cleared|=m_Value;
if(!cleared)
	return false;
m_Value=nullptr;
return true;
}

VOID XmlNode::InsertChildInternal(UINT pos, XmlNode* child)
{
if(!child)
	throw InvalidArgumentException();
auto name=child->GetName();
if(name)
	{
	if(m_Index.contains(name))
		throw AlreadyExistsException();
	}
m_Children.insert_at(pos, child);
if(name)
	m_Index.set(name, child);
}

VOID XmlNode::RemoveAttributeInternal(UINT pos)
{
auto const& att=m_Attributes.get_at(pos);
auto const& key=att.get_key();
RemoveAttributeInternal(key);
}

BOOL XmlNode::RemoveAttributeInternal(Handle<String> key)
{
if(StringHelper::Compare(key, "Name", 0, false)==0)
	return SetNameInternal(nullptr);
return m_Attributes.remove(key);
}

VOID XmlNode::RemoveChildInternal(UINT pos)
{
auto child=m_Children.get_at(pos);
auto name=child->GetName();
if(name)
	m_Index.remove(name);
m_Children.remove_at(pos);
}

VOID XmlNode::SetAttributeInternal(UINT pos, Handle<String> value)
{
auto const& att=m_Attributes.get_at(pos);
auto const& key=att.get_key();
SetAttributeInternal(key, value);
}

BOOL XmlNode::SetAttributeInternal(Handle<String> key, Handle<String> value)
{
if(StringHelper::Compare(key, "Name", 0, false)==0)
	return SetNameInternal(value);
return m_Attributes.set(key, value);
}

BOOL XmlNode::SetNameInternal(Handle<String> name)
{
Handle<String> old_name;
m_Attributes.try_get("Name", &old_name);
if(old_name==name)
	return false;
if(m_Parent)
	{
	WriteLock parent_lock(m_Parent->m_Mutex);
	if(name)
		{
		if(!m_Parent->m_Index.add(name, this))
			throw AlreadyExistsException();
		}
	if(old_name)
		m_Parent->m_Index.remove(old_name);
	}
if(name)
	{
	m_Attributes.set("Name", name);
	}
else
	{
	m_Attributes.remove("Name");
	}
return true;
}

BOOL XmlNode::SetTagInternal(Handle<String> tag)
{
if(m_Tag==tag)
	return false;
m_Tag=tag;
return true;
}

BOOL XmlNode::SetValueInternal(Handle<String> value)
{
assert(!m_Children);
if(m_Value==value)
	return false;
m_Value=value;
return true;
}

}}