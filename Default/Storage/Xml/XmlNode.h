//===========
// XmlNode.h
//===========

#pragma once


//=======
// Using
//=======

#include "Collections/list.hpp"
#include "Collections/map.hpp"
#include "Concurrency/Mutex.h"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
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

class XmlNode: public Object
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	friend XmlNodeAttributeIterator;
	friend XmlNodeChildIterator;

	// Con-/Destructors
	static inline Handle<XmlNode> Create(Handle<String> Tag=nullptr) { return new XmlNode(nullptr, Tag); }
	static inline Handle<XmlNode> Create(XmlNode* Parent, Handle<String> Tag=nullptr) { return new XmlNode(Parent, Tag); }

	// Common
	virtual VOID AppendChild(XmlNode* Child);
	Event<XmlNode> Changed;
	virtual BOOL Clear();
	virtual VOID CopyFrom(XmlNode* Node);
	Handle<String> GetAttribute(Handle<String> Key);
	BOOL GetAttribute(Handle<String> Key, UINT* Value);
	BOOL GetAttribute(Handle<String> Key, UINT64* Value);
	Handle<XmlNodeAttributeIterator> GetAttributes();
	Handle<XmlNode> GetChild(Handle<String> Name);
	Handle<XmlNode> GetChildAt(UINT Position);
	Handle<XmlNodeChildIterator> GetChildren();
	Handle<String> GetName();
	Handle<String> GetTag();
	Handle<String> GetValue();
	BOOL HasAttribute(Handle<String> Key);
	virtual VOID InsertChildAt(UINT Position, XmlNode* Child);
	SIZE_T ReadFromStream(InputStream* Stream);
	virtual BOOL RemoveAttribute(Handle<String> Key);
	virtual VOID RemoveChildAt(UINT Position);
	inline BOOL SetAttribute(Handle<String> Key, INT Value)
		{
		return SetAttribute(Key, String::Create("%i", Value));
		}
	inline BOOL SetAttribute(Handle<String> Key, INT64 Value)
		{
		return SetAttribute(Key, String::Create("%i", Value));
		}
	virtual BOOL SetAttribute(Handle<String> Key, Handle<String> Value);
	inline BOOL SetName(Handle<String> Name) { return SetAttribute("Name", Name); }
	virtual BOOL SetTag(Handle<String> Tag);
	virtual BOOL SetValue(Handle<String> Value);
	SIZE_T WriteToStream(OutputStream* Stream, INT Level=-1);

protected:
	// Con-/Destructors
	XmlNode(XmlNode* Parent=nullptr, Handle<String> Tag=nullptr);

	// Common
	VOID AppendChildInternal(XmlNode* Child);
	BOOL ClearInternal();
	virtual Handle<XmlNode> CreateNode();
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

class XmlNodeAttributeIterator: public Object
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;

	// Friends
	friend XmlNode;

	// Access
	inline Handle<String> GetKey()const { return m_It.get_key(); }
	inline Handle<String> GetValue()const { return m_It.get_value(); }
	inline BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	inline BOOL Begin() { return m_It.begin(); }
	inline BOOL End() { return m_It.rbegin(); }
	inline UINT GetPosition() { return m_It.get_position(); }
	inline BOOL MoveNext() { return m_It.move_next(); }
	inline BOOL MovePrevious() { return m_It.move_previous(); }

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

class XmlNodeChildIterator: public Object
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;

	// Friends
	friend XmlNode;

	// Access
	inline Handle<XmlNode> GetCurrent()const { return m_It.get_current(); }
	inline BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	inline BOOL Begin() { return m_It.begin(); }
	inline BOOL End() { return m_It.rbegin(); }
	inline UINT GetPosition() { return m_It.get_position(); }
	inline BOOL MoveNext() { return m_It.move_next(); }
	inline BOOL MovePrevious() { return m_It.move_previous(); }

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