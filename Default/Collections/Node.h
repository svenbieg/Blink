//========
// Node.h
//========

#pragma once


//=======
// Using
//=======

#include "Map.h"


//===========
// Namespace
//===========

namespace Collections {


//======
// Node
//======

class Node: public Object
{
public:
	// Using
	using ChildMap=Map<Handle<String>, Handle<Node>>;
	using PropertyMap=Map<Handle<String>, Handle<Object>>;

	// Con-/Destructors
	static inline Handle<Node> Create(Handle<String> Name=nullptr) { return new Node(Name); }

	// Common
	Handle<ChildMap> Children;
	Handle<Object> GetProperty(LPCSTR Path);
	Handle<String> Name;
	Handle<PropertyMap> Properties;

private:
	// Con-/Destructors
	Node(Handle<String> Name=nullptr);
};

}
