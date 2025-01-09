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

	// Con-/Destructors
	~DeviceTree();

	// Common
	static Handle<DeviceTree> Get();
	Handle<Object> GetProperty(LPCSTR Path);

private:
	// Con-/Destructors
	DeviceTree();

	// Common
	Handle<Node> CreateNode(SIZE_T* Offset);
	SIZE_T m_Offset;
	Handle<Node> m_Root;
	static DeviceTree* s_Current;
};

}
