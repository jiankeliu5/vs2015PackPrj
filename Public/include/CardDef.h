#pragma once

#define WM_USER 0x400
#define WM_INIT_USB_READER		(WM_USER + 30000)
#define WM_BEEP_USB_CARD_READER	(WM_USER + 30001)
#define WM_LED_TIP_MSG_CARD_READER (WM_USER + 30002)
#define WM_BEEP_USB_CARD_READER_Times (WM_USER + 30003)	//add by zfq,2013.11.20,¶Á¿¨Æ÷Á¬ÐøÃù½Ð
#define WM_ZKFPM_REQTEMPLETEDATA (WM_USER + 30010)
#define WM_ZKFPM_REGISTERTEMPLETE (WM_USER + 30011)
#define WM_ZKFPM_TEMPLETEADDED (WM_USER + 30012)
#define WM_IRIS_REQTEMPLETEDATA (WM_USER + 30013)
#define WM_IRIS_REGISTERTEMPLETE (WM_USER + 30014)
#define WM_IRIS_TEMPLETEADDED (WM_USER + 30015)
#define WM_FACEID_CHECK_USER (WM_USER + 30016)

struct IUdpCardCallback
{
public:
	virtual void OnReadCard(CString sIP, CString sNo) { ASSERT(0); }
	virtual void OnReadFace(CString sIP, int nFaceID) { ASSERT(0); }
	virtual void OnReadFinger(int nFingerID, CString sFingerStr) { ASSERT(0); }
	virtual void OnReadIris(int nFlag, BYTE* pTmpBuf, int nSize) { ASSERT(0); }
	virtual void OnCaptureIris(CString szFilePath, DWORD dwErr) { ASSERT(0); }
	virtual void OnFingerRegisted(int nTid, CString info) { ASSERT(0); }
};

struct IFaceIDCallBack
{
public:
	virtual void OnReadFace(CString sIP, int nFaceID) { ASSERT(0); }
};