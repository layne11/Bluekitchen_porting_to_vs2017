#pragma once
#include "afxdlgs.h"


/*---- class define £ºCProp *----*/
class CProp : public CPropertyPage
{
	DECLARE_DYNAMIC(CProp)

public:
	CProp(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize);
	CProp(LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize);
	virtual ~CProp();
	virtual BOOL OnSetActive();

protected:
	DECLARE_MESSAGE_MAP()
//	DECLARE_MESSAGE_MAP()
//public:
	//hfp msg
	afx_msg void OnHfpBnClickedBtnConn();
	afx_msg void OnHfpBnClickedBtnDisConn();
	afx_msg void OnHfpBnClickedBtnAnswer();
	afx_msg void OnHfpBnClickedBtnReject();
	afx_msg void OnHfpBnClickedBtnHangup();
	afx_msg void OnHfpBnClickedBtnDial();
	//a2dp msg
	afx_msg void OnA2dpBnClickedBtnConn();
	afx_msg void OnA2dpBnClickedBtnDisConn();
	//avrcp msg
	afx_msg void OnAvrcpBnClickedBtnConn();
	afx_msg void OnAvrcpBnClickedBtnDisConn(); 
	afx_msg void OnAvrcpBnClickedBtnNext();
	afx_msg void OnAvrcpBnClickedBtnPrev();
	afx_msg void OnAvrcpBnClickedBtnPlay();
	afx_msg void OnAvrcpBnClickedBtnPause();
	afx_msg void OnAvrcpBnClickedBtnStop();
	//gatt msg
	afx_msg void OnGattBnClickedBtnConn();
	afx_msg void OnGattBnClickedBtnDisConn();
	afx_msg void OnGattBnClickedBtnSend();
	//spp msg
	afx_msg void OnSppBnClickedBtnConn();
	afx_msg void OnSppBnClickedBtnDisConn();
	afx_msg void OnSppBnClickedBtnSend();
	//hid msg
	//afx_msg void OnHidBnClickedBtnPropB1();
};

class CPropSheet : public CPropertySheet
{
public:
	CPropSheet(UINT num, UINT *id, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropSheet(UINT num, UINT *id, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	~CPropSheet();

public:
	CProp **m_prop;

private:
	int PropPageNum;
};

void PropSheetInit(UINT page_num, UINT *page_id);


