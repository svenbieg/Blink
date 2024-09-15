//==============
// DeviceTree.h
//==============

#pragma once


//=======
// Using
//=======

#include "Collections/Node.h"


//===========
// Namespace
//===========

namespace Devices {


//=============
// Device-Tree
//=============

class DeviceTree: public Object
{
public:
	// Using
	using Node=Collections::Node;

	// Common
	Handle<Object> GetProperty(LPCSTR Path);
	static Handle<DeviceTree> Open();

private:
	// Con-/Destructors
	DeviceTree();

	// Common
	Handle<Node> CreateNode(SIZE_T* Offset);
	static Handle<DeviceTree> Current;
	SIZE_T Offset;
	Handle<Node> Root;
};

}
