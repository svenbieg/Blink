//==========
// Node.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Node.h"


//===========
// Namespace
//===========

namespace Collections {


//==================
// Con-/Destructors
//==================

Node::Node(Handle<String> name)
{
Children=new ChildMap();
Name=name;
Properties=new PropertyMap();
}


//========
// Common
//========

Handle<Object> Node::GetProperty(LPCSTR path)
{
if(!path||!path[0])
	return nullptr;
path=StringHelper::Truncate(path, "/\\:");
UINT dir_len=0;
if(StringHelper::FindChars(path, "/\\:", &dir_len))
	{
	Handle<String> dir_name=new String(dir_len, path);
	auto child=Children->Get(dir_name);
	if(!child)
		return nullptr;
	path+=dir_len+1;
	return child->GetProperty(path);
	}
return Properties->Get(path);
}

}
