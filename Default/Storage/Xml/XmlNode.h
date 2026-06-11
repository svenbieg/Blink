//===========
// XmlNode.h
//===========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Collections/list.hpp"
#include "Collections/map.hpp"
#include "Concurrency/Mutex.h"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "Storage/Xml/Xml.h"
#include "Event.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Xml {


//======================
// Forward-Declarations
//======================

class XmlNodeAttributeIterator;
class XmlNodeChildIterator;


//==========
// XML-Node
//==========

class XmlNode: public Xml
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	friend XmlNodeAttributeIterator;
	friend XmlNodeChildIterator;

	// Con-/Destructors
	static inline Handle<XmlNode> Create(Handle<String> Tag=nullptr)
		{
		return new XmlNode(nullptr, Tag);
		}
	static inline Handle<XmlNode> Create(XmlNode* Parent, Handle<String> Tag=nullptr)
		{
		return new XmlNode(Parent, Tag);
		}

	// Common
	VOID AppendChild(XmlNode* Child);
	Event<XmlNode> Changed;
	BOOL Clear();
	VOID CopyFrom(XmlNode* Node);
	Handle<String> GetAttribute(Handle<String> Key)override;
	BOOL GetAttribute(Handle<String> Key, Handle<String>* Value)override;
	BOOL GetAttribute(Handle<String> Key, UINT* Value);
	BOOL GetAttribute(Handle<String> Key, UINT64* Value);
	Handle<XmlAttributeIterator> GetAttributes()override;
	Handle<XmlNode> GetChild(Handle<String> Name);
	Handle<XmlNode> GetChildAt(UINT Position);
	Handle<XmlChildIterator> GetChildren()override;
	Handle<String> GetTag()override;
	Handle<String> GetValue()override;
	BOOL HasAttribute(Handle<String> Key)override;
	VOID InsertChildAt(UINT Position, XmlNode* Child);
	SIZE_T ReadFromStream(InputStream* Stream);
	BOOL RemoveAttribute(Handle<String> Key)override;
	VOID RemoveChildAt(UINT Position);
	BOOL SetAttribute(Handle<String> Key, Handle<String> Value)override;
	BOOL SetTag(Handle<String> Tag)override;
	BOOL SetValue(Handle<String> Value)override;
	SIZE_T WriteToStream(OutputStream* Stream, INT Level=-1);

protected:
	// Con-/Destructors
	XmlNode(XmlNode* Parent=nullptr, Handle<String> Tag=nullptr);

	// Common
	VOID AppendChildInternal(XmlNode* Child);
	BOOL ClearInternal();
	VOID InsertChildInternal(UINT Position, XmlNode* Child);
	VOID RemoveAttributeInternal(UINT Position);
	BOOL RemoveAttributeInternal(Handle<String> Key);
	VOID RemoveChildInternal(UINT Position);
	VOID SetAttributeInternal(UINT Position, Handle<String> Value);
	BOOL SetAttributeInternal(Handle<String> Key, Handle<String> Value);
	BOOL SetNameInternal(Handle<String> Name);
	BOOL SetTagInternal(Handle<String> Tag);
	BOOL SetValueInternal(Handle<String> Value);
	Collections::map<Handle<String>, Handle<String>, UINT> m_Attributes;
	Collections::list<Handle<XmlNode>, UINT> m_Children;
	Collections::map<Handle<String>, XmlNode*, UINT> m_Index;
	Concurrency::Mutex m_Mutex;
	XmlNode* m_Parent;
	Handle<String> m_Tag;
	Handle<String> m_Value;
};


//====================
// Attribute-Iterator
//====================

class XmlNodeAttributeIterator: public XmlAttributeIterator
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;

	// Friends
	friend XmlNode;

	// Access
	Handle<String> GetKey()const override { return m_It.get_key(); }
	Handle<String> GetValue()const override { return m_It.get_value(); }
	BOOL HasCurrent()const override { return m_It.has_current(); }

	// Navigation
	BOOL Begin()override { return m_It.begin(); }
	BOOL End()override { return m_It.rbegin(); }
	UINT GetPosition() { return m_It.get_position(); }
	BOOL MoveNext()override { return m_It.move_next(); }
	BOOL MovePrevious()override { return m_It.move_previous(); }

private:
	// Con-/Destructors
	XmlNodeAttributeIterator(XmlNode* Node): m_It(&Node->m_Attributes), m_Node(Node)
		{
		m_Node->m_Mutex.Lock(AccessMode::ReadOnly);
		}
	~XmlNodeAttributeIterator()
		{
		m_Node->m_Mutex.Unlock(AccessMode::ReadOnly);
		}

	// Common
	typename Collections::map<Handle<String>, Handle<String>, UINT>::iterator m_It;
	Handle<XmlNode> m_Node;
};


//================
// Child-Iterator
//================

class XmlNodeChildIterator: public XmlChildIterator
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;

	// Friends
	friend XmlNode;

	// Access
	Handle<Xml> GetCurrent()const override { return m_It.get_current(); }
	BOOL HasCurrent()const override { return m_It.has_current(); }

	// Navigation
	BOOL Begin()override { return m_It.begin(); }
	BOOL End()override { return m_It.rbegin(); }
	UINT GetPosition() { return m_It.get_position(); }
	BOOL MoveNext()override { return m_It.move_next(); }
	BOOL MovePrevious()override { return m_It.move_previous(); }

protected:
	// Con-/Destructors
	XmlNodeChildIterator(XmlNode* Node): m_It(&Node->m_Children), m_Node(Node)
		{
		m_Node->m_Mutex.Lock(AccessMode::ReadOnly);
		}
	~XmlNodeChildIterator()
		{
		m_Node->m_Mutex.Unlock(AccessMode::ReadOnly);
		}

	// Common
	typename Collections::list<Handle<XmlNode>, UINT>::iterator m_It;
	Handle<XmlNode> m_Node;
};

}}