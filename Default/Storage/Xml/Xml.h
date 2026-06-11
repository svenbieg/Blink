//=======
// Xml.h
//=======

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Xml {


//======================
// Forward-Declarations
//======================

class XmlAttributeIterator;
class XmlChildIterator;


//=====
// XML
//=====

class Xml: public virtual Object
{
public:
	// Friends
	friend XmlAttributeIterator;
	friend XmlChildIterator;

	// Common
	virtual Handle<String> GetAttribute(Handle<String> Key)=0;
	virtual BOOL GetAttribute(Handle<String> Key, Handle<String>* Value)=0;
	virtual Handle<XmlAttributeIterator> GetAttributes()=0;
	virtual Handle<XmlChildIterator> GetChildren()=0;
	virtual Handle<String> GetTag()=0;
	virtual Handle<String> GetValue()=0;
	virtual BOOL HasAttribute(Handle<String> Key)=0;
	virtual BOOL RemoveAttribute(Handle<String> Key)=0;
	virtual BOOL SetAttribute(Handle<String> Key, Handle<String> Value)=0;
	virtual BOOL SetTag(Handle<String> Tag)=0;
	virtual BOOL SetValue(Handle<String> Value)=0;

protected:
	// Con-/Destructors
	Xml()=default;
};


//====================
// Attribute-Iterator
//====================

class XmlAttributeIterator: public Object
{
public:
	// Access
	virtual Handle<String> GetKey()const=0;
	virtual Handle<String> GetValue()const=0;
	virtual BOOL HasCurrent()const=0;

	// Navigation
	virtual BOOL Begin()=0;
	virtual BOOL End()=0;
	virtual BOOL MoveNext()=0;
	virtual BOOL MovePrevious()=0;
};


//================
// Child-Iterator
//================

class XmlChildIterator: public Object
{
public:
	// Access
	virtual Handle<Xml> GetCurrent()const=0;
	virtual BOOL HasCurrent()const=0;

	// Navigation
	virtual BOOL Begin()=0;
	virtual BOOL End()=0;
	virtual BOOL MoveNext()=0;
	virtual BOOL MovePrevious()=0;
};

}}