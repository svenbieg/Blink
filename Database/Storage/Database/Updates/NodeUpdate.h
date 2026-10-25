//==============
// NodeUpdate.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Database/Updates/EntryUpdate.h"


//======================
// Forward-Declarations
//======================

namespace Storage
{
namespace Database
	{
	class Node;
	}
}


//===========
// Namespace
//===========

namespace Storage {
	namespace Database {
		namespace Updates {


//=========
// Updates
//=========

enum class NodeUpdateId: BYTE
{
None=0,
AttributeRemove,
AttributeRemoveAt,
AttributeSet,
AttributeSetAt,
ChildAppend,
ChildRemove,
Clear,
TagSet,
ValueSet
};


//=============
// Node-Update
//=============

class NodeUpdate: public EntryUpdate
{
public:
	// Friends
	friend Node;

protected:
	// Con-/Destructors
	NodeUpdate(Node* Node);

	// Common
	BOOL AttributeIndex(Handle<String> Key, UINT* Id);
	static SIZE_T ReadFromStream(InputStream* Stream, Node* Node, EntryUpdate** Update=nullptr);
	Node* m_Node;
};


//==================
// Attribute-Remove
//==================

class NodeUpdateAttributeRemove: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	inline Handle<String> GetKey()const { return m_Key; }
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateAttributeRemove(Node* Node, Handle<String> Key);
	static VOID Create(Node* Node, Handle<String> Key);

	// Common
	Handle<String> m_Key;
};


//===============
// Attribute-Set
//===============

class NodeUpdateAttributeSet: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	inline Handle<String> GetKey()const { return m_Key; }
	inline VOID SetValue(Handle<String> Value) { m_Value=Value; }
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateAttributeSet(Node* Node, Handle<String> Key, Handle<String> Value);
	static VOID Create(Node* Node, Handle<String> Key, Handle<String> Value);

	// Common
	static SIZE_T WriteToStream(OutputStream* Stream, UINT Position, Handle<String> Value);
	static SIZE_T WriteToStream(OutputStream* Stream, Handle<String> Key, Handle<String> Value);
	Handle<String> m_Key;
	Handle<String> m_Value;
};


//==============
// Child-Append
//==============

class NodeUpdateChildAppend: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateChildAppend(Node* Parent, Node* Child);
	static VOID Create(Node* Parent, Node* Child);

	// Common
	static SIZE_T WriteToStream(OutputStream* Stream, Node* Child);
	Handle<Node> m_Child;
};


//==============
// Child-Remove
//==============

class NodeUpdateChildRemove: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateChildRemove(Node* Node, UINT Child);
	static VOID Create(Node* Node, UINT Child);

	// Common
	UINT m_Child;
};


//=======
// Clear
//=======

class NodeUpdateClear: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateClear(Node* Node);
	static VOID Create(Node* Node);
};


//=========
// Tag-Set
//=========

class NodeUpdateTagSet: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateTagSet(Node* Node, Handle<String> Tag);
	static VOID Create(Node* Node, Handle<String> Tag);

	// Common
	static SIZE_T WriteToStream(OutputStream* Stream, Handle<String> Tag);
	Handle<String> m_Tag;
};


//===========
// Value-Set
//===========

class NodeUpdateValueSet: public NodeUpdate
{
public:
	// Friends
	friend Node;
	friend NodeUpdate;

	// Common
	SIZE_T WriteToStream(OutputStream* Stream)override;

private:
	// Con-/Destructors
	NodeUpdateValueSet(Node* Node, Handle<String> Value);
	static VOID Create(Node* Node, Handle<String> Value);

	// Common
	static SIZE_T WriteToStream(OutputStream* Stream, INT64 Value);
	static SIZE_T WriteToStream(OutputStream* Stream, Handle<String> Value);
	Handle<String> m_Value;
};

}}}