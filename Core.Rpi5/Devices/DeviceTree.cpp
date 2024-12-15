//================
// DeviceTree.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Buffer.h"
#include "DeviceTree.h"

using namespace Collections;
using namespace Storage;


//===========
// Namespace
//===========

namespace Devices {


//==========
// Settings
//==========

constexpr SIZE_T DTB_OFFSET_PTR32=0xF8;


//========
// Header
//========

typedef struct
{
UINT MAGIC;
UINT SIZE;
UINT OFF_DT_STRUCT;
UINT OFF_DT_STRINGS;
UINT OFF_MEM_RSVMAP;
UINT VERSION;
UINT VERSION_LAST;
UINT BOOT_CPUUID_PHYS;
UINT SIZE_DT_STRINGS;
UINT SIZE_DT_STRUCT;
}DTB_HEADER;

constexpr UINT DTB_MAGIC=0xD00DFEED;


//======
// Node
//======

typedef struct
{
UINT TOKEN;
BYTE DATA[];
}DTB_NODE;


//==========
// Property
//==========

typedef struct
{
UINT TOKEN;
UINT LEN;
UINT NAME_OFF;
BYTE DATA[];
}DTB_PROPERTY;


//=======
// Token
//=======

typedef enum
{
TOKEN_BEGIN=1,
TOKEN_END,
TOKEN_PROP,
TOKEN_NOP,
TOKEN_STOP=9
}DTB_TOKEN;


//========
// Common
//========

Handle<Object> DeviceTree::GetProperty(LPCSTR path)
{
if(!Root)
	return nullptr;
return Root->GetProperty(path);
}

Handle<DeviceTree> DeviceTree::Open()
{
if(!Current)
	Current=new DeviceTree();
return Current;
}


//==========================
// Con-/Destructors Private
//==========================

DeviceTree::DeviceTree()
{
auto offset_ptr=(UINT*)DTB_OFFSET_PTR32;
Offset=*offset_ptr;
auto header=(DTB_HEADER*)Offset;
if(BigEndian(header->MAGIC)!=DTB_MAGIC)
	return;
SIZE_T offset=Offset+BigEndian(header->OFF_DT_STRUCT);
Root=CreateNode(&offset);
}


//================
// Common Private
//================

Handle<Node> DeviceTree::CreateNode(SIZE_T* offset_ptr)
{
SIZE_T node_pos=*offset_ptr;
auto dtb_node=(DTB_NODE*)node_pos;
while(BigEndian(dtb_node->TOKEN)==TOKEN_NOP)
	{
	node_pos+=sizeof(UINT);
	dtb_node=(DTB_NODE*)node_pos;
	}
if(BigEndian(dtb_node->TOKEN)!=TOKEN_BEGIN)
	return nullptr;
LPCSTR name_ptr=(LPCSTR)dtb_node->DATA;
UINT name_len=StringHelper::Length(name_ptr);
Handle<String> name;
if(name_len>0)
	name=new String(name_ptr);
Handle<Node> node=new Node(name);
auto header=(DTB_HEADER*)Offset;
SIZE_T strings_pos=Offset+BigEndian(header->OFF_DT_STRINGS);
node_pos+=sizeof(UINT)+AlignUp(name_len+1, 4);
while(1)
	{
	dtb_node=(DTB_NODE*)node_pos;
	if(BigEndian(dtb_node->TOKEN)==TOKEN_NOP)
		{
		node_pos+=sizeof(UINT);
		continue;
		}
	if(BigEndian(dtb_node->TOKEN)==TOKEN_PROP)
		{
		auto prop=(DTB_PROPERTY*)node_pos;
		SIZE_T name_pos=strings_pos+BigEndian(prop->NAME_OFF);
		LPCSTR name_ptr=(LPCSTR)name_pos;
		Handle<String> prop_name=new String(name_ptr);
		UINT prop_len=BigEndian(prop->LEN);
		Handle<Buffer> prop_value=new Buffer(prop->DATA, prop_len);
		node->Properties->Add(prop_name, prop_value);
		node_pos+=sizeof(DTB_PROPERTY)+AlignUp(prop_len, 4);
		continue;
		}
	if(BigEndian(dtb_node->TOKEN)==TOKEN_BEGIN)
		{
		auto child=CreateNode(&node_pos);
		if(!child||!child->Name)
			return nullptr;
		node->Children->Add(child->Name, child);
		continue;
		}
	if(BigEndian(dtb_node->TOKEN)==TOKEN_END)
		{
		node_pos+=sizeof(UINT);
		break;
		}
	return nullptr;
	}
*offset_ptr=node_pos;
return node;
}

Handle<DeviceTree> DeviceTree::Current;

}
