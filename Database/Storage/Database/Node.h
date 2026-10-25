//========
// Node.h
//========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database

#pragma once


//=======
// Using
//=======

#include "Collections/list.hpp"
#include "Collections/map.hpp"
#include "Storage/Database/Updates/NodeUpdate.h"
#include "Storage/Database/Entry.h"
#include "Storage/Xml/Xml.h"
#include "Storage/File.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {


//======================
// Forward-Declarations
//======================

class Database;
class Editor;
class NodeAttributeIterator;
class NodeChildIterator;


//======
// Node
//======

class Node: public Entry, public Storage::Xml::Xml
{
public:
	// Using
	using AttributeIndex=Collections::index<Handle<String>, UINT>;
	using AttributeMap=Collections::map<Handle<String>, Handle<String>, UINT>;
	using ChildList=Collections::list<Handle<Node>, UINT>;
	using NodeUpdate=Storage::Database::Updates::NodeUpdate;
	using XmlAttributeIterator=Storage::Xml::XmlAttributeIterator;
	using XmlChildIterator=Storage::Xml::XmlChildIterator;

	// Friends
	friend Database;
	friend NodeAttributeIterator;
	friend NodeChildIterator;
	friend NodeUpdate;
	friend Object;

	// Common
	BOOL Clear();
	BOOL Clear(Editor* Editor);
	Handle<String> GetAttribute(Handle<String> Key)override;
	BOOL GetAttribute(Handle<String> Key, Handle<String>* Value)override;
	Handle<XmlAttributeIterator> GetAttributes()override;
	Handle<Node> GetChild(Handle<String> Tag);
	Handle<Node> GetChildAt(UINT Position);
	UINT GetChildCount()override;
	Handle<XmlChildIterator> GetChildren()override;
	Handle<String> GetTag()override;
	Handle<String> GetValue()override;
	BOOL HasAttribute(Handle<String> Key)override;
	SIZE_T ReadFromStream(InputStream* Stream);
	BOOL RemoveAttribute(Handle<String> Key)override;
	BOOL RemoveAttribute(Editor* Editor, Handle<String> Key);
	VOID RemoveChild(Handle<Node> Child);
	VOID RemoveChild(Editor* Editor, Handle<Node> Child);
	BOOL SetAttribute(Handle<String> Key, Handle<String> Value)override;
	BOOL SetAttribute(Editor* Editor, Handle<String> Key, Handle<String> Value);
	BOOL SetTag(Handle<String> Tag)override;
	BOOL SetTag(Editor* Editor, Handle<String> Tag);
	BOOL SetValue(Handle<String> Value)override;
	BOOL SetValue(Editor* Editor, Handle<String> Value);
	SIZE_T WriteToStream(OutputStream* Stream);

protected:
	// Con-/Destructors
	Node(Database* Database, UINT Id);
	Node(Database* Database, Handle<String> Tag=nullptr);
	Node(Node* Parent, Handle<String> Tag=nullptr);
	static inline Handle<Node> Create(Database* Database, UINT Block)
		{
		return Entry::Create<Node>(Database, Block);
		}
	static inline Handle<Node> Create(Database* Database, Handle<String> Tag=nullptr)
		{
		return Object::Create<Node>(Database, Tag);
		}
	static inline Handle<Node> Create(Node* Parent, Handle<String> Tag=nullptr)
		{
		assert(Parent);
		return Object::Create<Node>(Parent, Tag);
		}

	// Common
	SIZE_T ReadEntry(InputStream* Stream)override;
	SIZE_T WriteEntry(OutputStream* Stream)override;

private:
	// Settings
	static const UINT NODE_TYPE=ENTRY_TYPE('NODE');

	// Common
	BOOL ClearInternal(Editor* Editor);
	AttributeIndex m_AttributeIndex;
	AttributeMap m_Attributes;
	ChildList m_Children;
	Handle<String> m_Tag;
	Handle<String> m_Value;
};


//====================
// Attribute-Iterator
//====================

class NodeAttributeIterator: public Storage::Xml::XmlAttributeIterator
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;

	// Friends
	friend Node;
	friend Object;

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
	BOOL SetPosition(UINT Position)override { return m_It.set_position(Position); }

private:
	// Con-/Destructors
	NodeAttributeIterator(Node* Node): m_It(&Node->m_Attributes), m_Node(Node)
		{
		m_Node->m_Mutex.Lock(AccessMode::ReadOnly);
		}
	~NodeAttributeIterator()
		{
		m_Node->m_Mutex.Unlock(AccessMode::ReadOnly);
		}
	static inline Handle<NodeAttributeIterator> Create(Node* Node)
		{
		return Object::Create<NodeAttributeIterator>(Node);
		}

	// Common
	typename Collections::map<Handle<String>, Handle<String>, UINT>::iterator m_It;
	Handle<Node> m_Node;
};


//================
// Child-Iterator
//================

class NodeChildIterator: public Storage::Xml::XmlChildIterator
{
public:
	// Using
	using AccessMode=Concurrency::AccessMode;
	using Xml=Storage::Xml::Xml;

	// Friends
	friend Node;
	friend Object;

	// Access
	Handle<Xml> GetCurrent()const override { return m_It.get_current(); }
	BOOL HasCurrent()const override { return m_It.has_current(); }

	// Navigation
	BOOL Begin()override { return m_It.begin(); }
	BOOL End()override { return m_It.rbegin(); }
	UINT GetPosition() { return m_It.get_position(); }
	BOOL MoveNext()override { return m_It.move_next(); }
	BOOL MovePrevious()override { return m_It.move_previous(); }
	BOOL SetPosition(UINT Position)override { return m_It.set_position(Position); }

protected:
	// Con-/Destructors
	NodeChildIterator(Node* Node): m_It(&Node->m_Children), m_Node(Node)
		{
		m_Node->m_Mutex.Lock(AccessMode::ReadOnly);
		}
	~NodeChildIterator()
		{
		m_Node->m_Mutex.Unlock(AccessMode::ReadOnly);
		}
	static inline Handle<NodeChildIterator> Create(Node* Node)
		{
		return Object::Create<NodeChildIterator>(Node);
		}

	// Common
	typename Collections::list<Handle<Node>, UINT>::iterator m_It;
	Handle<Node> m_Node;
};

}}