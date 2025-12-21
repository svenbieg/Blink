//==========
// Node.cpp
//==========

#include "Node.h"


//===========
// Namespace
//===========

namespace Collections {


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
	auto dir_name=String::Create(dir_len, path);
	auto child=Children->Get(dir_name);
	if(!child)
		return nullptr;
	path+=dir_len+1;
	return child->GetProperty(path);
	}
return Properties->Get(path);
}


//==========================
// Con-/Destructors Private
//==========================

Node::Node(Handle<String> name)
{
Children=ChildMap::Create();
Name=name;
Properties=PropertyMap::Create();
}

}