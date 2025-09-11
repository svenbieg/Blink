//=================
// StringBuilder.h
//=================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/OutputStream.h"


//================
// String-Builder
//================

class StringBuilder
{
public:
	// Con-/Destructors
	StringBuilder(UINT Length);

	// Common
	UINT Append(CHAR Char);
	UINT Append(WCHAR Char);
	UINT Append(LPCSTR String);
	UINT Append(LPCWSTR String);
	UINT Append(UINT Length, LPCSTR String);
	UINT Append(UINT Length, LPCWSTR String);
	Handle<String> ToString();

private:
	// Common
	template <class _char_t> UINT AppendChar(_char_t Char)
		{
		if(m_Position==m_Size)
			return 0;
		m_Buffer[m_Position++]=CharHelper::ToChar(Char);
		return 1;
		}
	template <class _char_t> UINT AppendString(UINT Length, _char_t const* String)
		{
		if(!String)
			return 0;
		LPTSTR buf=&m_Buffer[m_Position];
		UINT pos=0;
		for(; String[pos]; pos++)
			{
			if(pos==Length)
				break;
			if(m_Position==m_Size)
				break;
			buf[pos]=String[pos];
			m_Position++;
			}
		return pos;
		}
	LPTSTR m_Buffer;
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_String;
};
