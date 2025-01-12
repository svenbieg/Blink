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

class IpAddress: public Variable
{
public:
	// Con-/Destrucotrs
	static Handle<IpAddress> Create(IP_ADDR Value=0);
	static Handle<IpAddress> Create(BYTE A0, BYTE A1, BYTE A2, BYTE A3);
	static Handle<IpAddress> Create(Handle<String> Name, IP_ADDR Value=0);
	static Handle<IpAddress> Create(Handle<String> Name, BYTE A0, BYTE A1, BYTE A2, BYTE A3);

	// Access
	IP_ADDR Get();
	static inline IP_ADDR Get(IpAddress* Value) { return Value? Value->Get(): 0; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, IP_ADDR&> Reading;
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;
	static Handle<String> ToString(IP_ADDR Address);
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> Address, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> Address, IP_ADDR* Ip);
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(IP_ADDR Value, BOOL Notify=true);

	// Common
	static IP_ADDR From(BYTE A0, BYTE A1, BYTE A2, BYTE A3);

private:
	// Con-/Destrucotrs
	IpAddress(Handle<String> Name, IP_ADDR Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	IP_ADDR m_Value;
};

}}


//========
// Handle
//========

template <>
class Handle<Network::Ip::IpAddress>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Using
	using IP_ADDR=Network::Ip::IP_ADDR;
	using IpAddress=Network::Ip::IpAddress;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(IpAddress* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return IpAddress::Get(m_Object)!=0; }
	inline operator IpAddress*()const { return m_Object; }
	inline IpAddress* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return IpAddress::Get(m_Object)==0; }
	inline BOOL operator==(IpAddress* Value)const { return IpAddress::Get(m_Object)==IpAddress::Get(Value); }
	inline BOOL operator==(IP_ADDR Value)const { return IpAddress::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return IpAddress::Get(m_Object)!=0; }
	inline BOOL operator!=(IpAddress* Value)const { return IpAddress::Get(m_Object)!=IpAddress::Get(Value); }
	inline BOOL operator!=(IP_ADDR Value)const { return IpAddress::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return IpAddress::Get(m_Object)>0; }
	inline BOOL operator>(IpAddress* Value)const { return IpAddress::Get(m_Object)>IpAddress::Get(Value); }
	inline BOOL operator>(IP_ADDR Value)const { return IpAddress::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return IpAddress::Get(m_Object)>=0; }
	inline BOOL operator>=(IpAddress* Value)const { return IpAddress::Get(m_Object)>=IpAddress::Get(Value); }
	inline BOOL operator>=(IP_ADDR Value)const { return IpAddress::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return IpAddress::Get(m_Object)<0; }
	inline BOOL operator<(IpAddress* Value)const { return IpAddress::Get(m_Object)<IpAddress::Get(Value); }
	inline BOOL operator<(IP_ADDR Value)const { return IpAddress::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return IpAddress::Get(m_Object)<=0; }
	inline BOOL operator<=(IpAddress* Value)const { return IpAddress::Get(m_Object)<=IpAddress::Get(Value); }
	inline BOOL operator<=(IP_ADDR Value)const { return IpAddress::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(IpAddress* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	Handle& operator=(INT Value)
		{
		if(!m_Object)
			{
			auto value=IpAddress::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	IpAddress* m_Object;
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