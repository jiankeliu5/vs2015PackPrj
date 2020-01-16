#pragma once
#include "../eCardInfEx/eCardInfEx.h"

#ifdef _DEBUG
#pragma comment(lib,"..\\debug\\eCardInfEx.lib")
#else
#pragma comment(lib,"..\\release\\eCardInfEx.lib")
#endif

extern "C"  __declspec(dllimport)  int __stdcall  eCardInit();
extern "C"	__declspec(dllimport)  int __stdcall  eCardExit();
extern "C"	__declspec(dllimport)  int __stdcall  eCardBeep();
extern "C"  __declspec(dllimport)  int __stdcall  eCardGetCardNum(unsigned char* pCardNum, int nLen);
extern "C"  __declspec(dllimport)  int __stdcall  eCardGetUserInfo(PVOID);
extern "C"  __declspec(dllimport)  int __stdcall  eCardConsumeCardMoney(unsigned long cardaccount
	, int  nConsumeBalance	, CString &szConsumeDetail);
extern "C"  __declspec(dllimport)  int __stdcall  eCardGetBalance(unsigned long cardaccount, int& nBalance);

