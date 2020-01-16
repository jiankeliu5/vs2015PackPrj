#pragma once
#include "../eCardInf/eCardInf.h"
#include "../eCardInf/export.h"

#ifdef _DEBUG
#pragma comment(lib,"..\\debug\\eCardInf.lib")
#else
#pragma comment(lib,"..\\release\\eCardInf.lib")
#endif
extern "C"  __declspec(dllexport) int __stdcall InitCard();
extern "C"  __declspec(dllexport) int __stdcall GetCardNumber(char*	pCardNum, int& nSize);
extern "C"  __declspec(dllexport) int __stdcall GetCardDetail(CARD_DETAIL*	pInfo, char* szNum);
extern "C"  __declspec(dllexport) int __stdcall GetCardState(int& nState, char* szNum);
extern "C"  __declspec(dllexport) int __stdcall ReduceMoney(double fMoney, char* szNum);
extern "C"  __declspec(dllexport) int __stdcall ReduceMoneyA(double fMoney, char* szNum, int *pnRet);
extern "C"  __declspec(dllexport) int __stdcall ReduceMoneywithJobid(double fMoney, char* szNum, int *pnRet, int nJobid);
extern "C"  __declspec(dllexport) int __stdcall StopCard();
extern "C"  __declspec(dllexport) int __stdcall iSecPrint_GetAllUserInfo();
extern "C"  __declspec(dllexport) int __stdcall iSecPrint_IsSupportGetAll();
extern "C"  __declspec(dllexport) int __stdcall iSecPrint_IsValidCard(CARD_DETAIL* pInfo);
extern "C" __declspec(dllexport) int __stdcall GetCardDetailByCardNoAndPwd(CARD_DETAIL* pInfo, char* pCardNo, char* pPwd);
extern "C" __declspec(dllexport) int __stdcall ReduceMoneyByCardNoAndPwd(double fMoney, char* pCardNo, char* pPwd);
extern "C" __declspec(dllexport) int __stdcall SaveLastTransReq();
extern "C" __declspec(dllexport) int __stdcall SetUserName(char* username);
extern "C" __declspec(dllexport) int __stdcall ConsumeFinish(char* pCardNum);
extern "C"  __declspec(dllexport) int __stdcall CardBeep(int iSeconds);
extern "C" __declspec(dllexport) int __stdcall GetOneCardBalance(unsigned long cardaccount, int&  nBalance);
extern "C"  __declspec(dllexport) int __stdcall GetTradeParameters(char* pTradeParameters);


