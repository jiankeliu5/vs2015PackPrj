#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "logTypes.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <deque>
#include <queue>


_XABLOG_BEGIN

//! optimze from std::stringstream to ZLog4zStream
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif

class ZLog4zBinary
{
public:
	ZLog4zBinary(const char * buf, int len)
	{
		_buf = buf;
		_len = len;
	}
	const char * _buf;
	int  _len;
};

class ZLog4zStream
{
public:
	inline ZLog4zStream(char * buf, int len);
	inline int getCurrentLen() { return (int)(_cur - _begin); }
private:
	template<class T>
	inline ZLog4zStream & writeData(const char * ft, T t);
	inline ZLog4zStream & writeLongLong(long long t);
	inline ZLog4zStream & writeULongLong(unsigned long long t);
	inline ZLog4zStream & writePointer(const void * t);
	inline ZLog4zStream & writeString(const wchar_t* t) { return writeData("%s", t); }
	inline ZLog4zStream & writeWString(const wchar_t* t);
	inline ZLog4zStream & writeBinary(const ZLog4zBinary & t);
public:
	inline ZLog4zStream & operator <<(const void * t) { return  writePointer(t); }

	inline ZLog4zStream & operator <<(const char * t) { return writeData("%s", t); }
#ifdef WIN32
	inline ZLog4zStream & operator <<(const wchar_t * t) { return writeWString(t); }
#endif
	inline ZLog4zStream & operator <<(bool t) { return (t ? writeData("%s", "true") : writeData("%s", "false")); }

	inline ZLog4zStream & operator <<(char t) { return writeData("%c", t); }

	inline ZLog4zStream & operator <<(unsigned char t) { return writeData("%u", (unsigned int)t); }

	inline ZLog4zStream & operator <<(short t) { return writeData("%d", (int)t); }

	inline ZLog4zStream & operator <<(unsigned short t) { return writeData("%u", (unsigned int)t); }

	inline ZLog4zStream & operator <<(int t) { return writeData("%d", t); }

	inline ZLog4zStream & operator <<(unsigned int t) { return writeData("%u", t); }

	inline ZLog4zStream & operator <<(long t) { return writeLongLong(t); }

	inline ZLog4zStream & operator <<(unsigned long t) { return writeULongLong(t); }

	inline ZLog4zStream & operator <<(long long t) { return writeLongLong(t); }

	inline ZLog4zStream & operator <<(unsigned long long t) { return writeULongLong(t); }

	inline ZLog4zStream & operator <<(float t) { return writeData("%.4f", t); }

	inline ZLog4zStream & operator <<(double t) { return writeData("%.4lf", t); }

	template<class _Elem, class _Traits, class _Alloc> //support std::string, std::wstring
	inline ZLog4zStream & operator <<(const std::basic_string<_Elem, _Traits, _Alloc> & t) { return *this << t.c_str(); }

	inline ZLog4zStream & operator << (const XabLog::ZLog4zBinary & binary) { return writeBinary(binary); }

	template<class _Elem, class _Alloc>
	inline ZLog4zStream & operator <<(const std::vector<_Elem, _Alloc> & t) { *this << "vector[size="; *this << t.size(); return *this << "]"; }
	template<class _Elem, class _Alloc>
	inline ZLog4zStream & operator <<(const std::list<_Elem, _Alloc> & t) { *this << "list[size="; *this << t.size(); return *this << "]"; }
	template<class _Elem, class _Alloc>
	inline ZLog4zStream & operator <<(const std::deque<_Elem, _Alloc> & t) { *this << "deque[size="; *this << t.size(); return *this << "]"; }
	template<class _Elem, class _Alloc>
	inline ZLog4zStream & operator <<(const std::queue<_Elem, _Alloc> & t) { *this << "queue[size="; *this << t.size(); return *this << "]"; }
	template<class _K, class _V, class _Pr, class _Alloc>
	inline ZLog4zStream & operator <<(const std::map<_K, _V, _Pr, _Alloc> & t) { *this << "map[size="; *this << t.size(); return *this << "]"; }

private:
	ZLog4zStream() {}
	ZLog4zStream(ZLog4zStream &) {}
	char *  _begin;
	char *  _end;
	char *  _cur;
};

inline ZLog4zStream::ZLog4zStream(char * buf, int len)
{
	_begin = buf;
	_end = buf + len;
	_cur = _begin;
}

template<class T>
inline ZLog4zStream& ZLog4zStream::writeData(const char * ft, T t)
{
	if (_cur < _end)
	{
		int len = 0;
		int count = (int)(_end - _cur);
#ifdef WIN32
		len = _snprintf(_cur, count, ft, t);
		if (len == count || (len == -1 && errno == ERANGE))
		{
			len = count;
			*(_end - 1) = '\0';
		}
		else if (len < 0)
		{
			*_cur = '\0';
			len = 0;
		}
#else
		len = snprintf(_cur, count, ft, t);
		if (len < 0)
		{
			*_cur = '\0';
			len = 0;
		}
		else if (len >= count)
		{
			len = count;
			*(_end - 1) = '\0';
		}
#endif
		_cur += len;
	}
	return *this;
}

inline ZLog4zStream & ZLog4zStream::writeLongLong(long long t)
{
#ifdef WIN32  
	writeData("%I64d", t);
#else
	writeData("%lld", t);
#endif
	return *this;
}

inline ZLog4zStream & ZLog4zStream::writeULongLong(unsigned long long t)
{
#ifdef WIN32  
	writeData("%I64u", t);
#else
	writeData("%llu", t);
#endif
	return *this;
}

inline ZLog4zStream & ZLog4zStream::writePointer(const void * t)
{
#ifdef WIN32
	sizeof(t) == 8 ? writeData("%016I64x", (unsigned long long)t) : writeData("%08I64x", (unsigned long long)t);
#else
	sizeof(t) == 8 ? writeData("%016llx", (unsigned long long)t) : writeData("%08llx", (unsigned long long)t);
#endif
	return *this;
}

inline ZLog4zStream & ZLog4zStream::writeBinary(const ZLog4zBinary & t)
{
	writeData("%s", "\r\n\t[");
	for (int i = 0; i < t._len; i++)
	{
		if (i % 16 == 0)
		{
			writeData("%s", "\r\n\t");
			*this << (void*)(t._buf + i);
			writeData("%s", ": ");
		}
		writeData("%02x ", (unsigned char)t._buf[i]);
	}
	writeData("%s", "\r\n\t]\r\n\t");
	return *this;
}

inline XabLog::ZLog4zStream & XabLog::ZLog4zStream::writeWString(const wchar_t* t)
{
#ifdef WIN32
	DWORD dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
	if (dwLen < LOG4Z_LOG_BUF_SIZE)
	{
		std::string str;
		str.resize(dwLen, '\0');
		dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, &str[0], dwLen, NULL, NULL);
		if (dwLen > 0)
		{
			writeData("%s", str.c_str());
		}
	}
#else
	//not support
#endif
	return *this;
}


#ifdef WIN32
#pragma warning(pop)
#endif

_XABLOG_END
