//=============
// IpAddress.h
//=============

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ip {


//======
// Type
//======

typedef UINT IP_ADDR;


//========
// Common
//========

constexpr IP_ADDR IP_BROADCAST=0xFFFFFFFF;


//============
// IP-Address
//============

class IpAddress: public TypedVariable<IP_ADDR>
{
public:
	// Con-/Destrucotrs
	static Handle<IpAddress> Create(IP_ADDR Address=0);
	static Handle<IpAddress> Create(BYTE A0, BYTE A1, BYTE A2, BYTE A3);
	static Handle<IpAddress> Create(Handle<String> Name, IP_ADDR Address=0);
	static Handle<IpAddress> Create(Handle<String> Name, BYTE A0, BYTE A1, BYTE A2, BYTE A3);

	// Access
	operator IP_ADDR() { return Get(); }
	Handle<String> ToString()override;
	static Handle<String> ToString(IP_ADDR Address);

	// Modification
	BOOL FromString(Handle<String> Address, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> Address, IP_ADDR* Ip);

	// Common
	static IP_ADDR From(BYTE A0, BYTE A1, BYTE A2, BYTE A3);

private:
	// Con-/Destrucotrs
	IpAddress(Handle<String> Name, IP_ADDR Address): TypedVariable(Name, Address) {}
};

}}


//===================
// Handle IP-Address
//===================

template <>
class Handle<Network::Ip::IpAddress>: public VariableHandle<Network::Ip::IpAddress, Network::Ip::IP_ADDR>
{
public:
	// Using
	using IP_ADDR=Network::Ip::IP_ADDR;
	using _base_t=VariableHandle<Network::Ip::IpAddress, IP_ADDR>;
	using _base_t::_base_t;

	// Modification
	inline Handle& operator=(IP_ADDR const& Value) { Set(Value); return *this; }
};


//==================
// Con-/Destructors
//==================

namespace Network {
	namespace Ip {

inline Handle<IpAddress> IpAddress::Create(IP_ADDR Address)
{
return new IpAddress(nullptr, Address);
}

inline Handle<IpAddress> IpAddress::Create(BYTE A0, BYTE A1, BYTE A2, BYTE A3)
{
auto ip=IpAddress::From(A0, A1, A2, A3);
return new IpAddress(nullptr, ip);
}

inline Handle<IpAddress> IpAddress::Create(Handle<String> Name, IP_ADDR Address)
{
return new IpAddress(Name, Address);
}

inline Handle<IpAddress> IpAddress::Create(Handle<String> Name, BYTE A0, BYTE A1, BYTE A2, BYTE A3)
{
auto ip=IpAddress::From(A0, A1, A2, A3);
return new IpAddress(Name, ip);
}

}}