#include "stdafx.h"
#include "PropSheet.h"
#include "btstack_demo.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	#include "app.h"
#ifdef __cplusplus
}
#endif
CPropSheet *pSheet = NULL;
UINT *pPropPageId = NULL;
UINT pPropPageIdlen = 0;
/*
	//HFP HF EVENT
	APP_EVT_HFP_HF_CONN,
	APP_EVT_HFP_HF_DISCONN,
	APP_EVT_HFP_HF_INCOMING_CALL,
	APP_EVT_HFP_HF_OUTGOING_CALL,
	APP_EVT_HFP_HF_CALLING,
	APP_EVT_HFP_HF_CALLING_PHONE,

	//A2DP SINK EVENT
	APP_EVT_A2DP_SINK_CONN,
	APP_EVT_A2DP_SINK_DISCONN,

	//AVRCP CT EVENT
	APP_EVT_AVRCP_CT_CONN,
	APP_EVT_AVRCP_CT_DISCONN,
	APP_EVT_AVRCP_CT_STOP,
	APP_EVT_AVRCP_CT_PLAY,
	APP_EVT_AVRCP_CT_PAUSE,

	//GATT SERVER EVENT
	APP_EVT_GATT_SER_CONN,
	APP_EVT_GATT_SER_DISCONN,
	APP_EVT_GATT_SER_DATA_RCV,

	//SPP SERVER EVENT
	APP_EVT_SPP_SER_CONN,
	APP_EVT_SPP_SER_DISCONN,
	APP_EVT_SPP_SER_DATA_RCV,
*/
const CString stack_display_evt_str_map[] = {
	//HFP
	_T("CONNECTED"),
	_T("IDLE"),
	_T("OUTGONIG..."),
	_T("INCOMING..."),
	_T("CALLING..."),
	_T("CALLING PHONE"),

	//A2DP
	_T("CONNECTED"),
	_T("IDLE"),

	//AVRCP
	_T("CONNECTED"),
	_T("IDLE"),
	_T("STOP"),
	_T("PLAY"),
	_T("PAUSE"),

	//GATT
	_T("CONNECTED"),
	_T("IDLE"),
	_T("CONNECTED"),

	//SPP
	_T("CONNECTED"),
	_T("IDLE"),
	_T("CONNECTED"),
};
void app_display(uint16_t evt, uint8_t *param, uint16_t len);

void string_to_str(char * c, CString s) {
	int i = 0;
	for (i = 0; i < s.GetLength(); i++)
	{
		c[i] = s[i];
	}
	c[i] = '\0';
}
void str_to_wchar_t_str(char * src, uint16_t slen, wchar_t *dst, uint16_t dlen)
{
	long lLen = MultiByteToWideChar(CP_ACP, 0, (char *)src, slen, dst, dlen);
	dst[lLen] = L'\0';
}
CProp *get_pclass_by_prop_pageid(UINT id)
{
	int i;
	for (i = 0; i < pPropPageIdlen; i++) {
		if (pPropPageId[i] == id)
			return pSheet->m_prop[i];
	}
	return NULL;
}
UINT get_prop_id_by_type(uint8_t type)
{
	switch (type) {
		case STACK_EVENT_TYPE_HFP:
			return IDD_PROP_HFP;
		case STACK_EVENT_TYPE_A2DP:
			return IDD_PROP_A2DP;
		case STACK_EVENT_TYPE_AVRCP:
			return IDD_PROP_AVRCP;
		case STACK_EVENT_TYPE_GATT:
			return IDD_PROP_GATT;
		case STACK_EVENT_TYPE_SPP:
			return IDD_PROP_SPP;
		case STACK_EVENT_TYPE_HID:
			return IDD_PROP_HID;
		default:
			return 0;
	}
}

uint8_t get_type_by_event(uint16_t evt)
{

		if (evt >= APP_EVT_HFP_HF_CONN && evt <= APP_EVT_HFP_HF_CALLING_PHONE) {
			return STACK_EVENT_TYPE_HFP;
		}else if (evt >= APP_EVT_A2DP_SINK_CONN && evt <= APP_EVT_A2DP_SINK_DISCONN) {
			return STACK_EVENT_TYPE_A2DP;
		}else if (evt >= APP_EVT_AVRCP_CT_CONN && evt <= APP_EVT_AVRCP_CT_PAUSE) {
			return STACK_EVENT_TYPE_AVRCP;
		}else if (evt >= APP_EVT_GATT_SER_CONN && evt <= APP_EVT_GATT_SER_DATA_RCV) {
			return STACK_EVENT_TYPE_GATT;
		}else if (evt >= APP_EVT_SPP_SER_CONN && evt <= APP_EVT_SPP_SER_DATA_RCV) {
			return STACK_EVENT_TYPE_SPP;
		}else if (evt >= APP_EVT_HID_SER_CONN && evt <= APP_EVT_HID_SER_REPORT_GAMEPAD) {
			return STACK_EVENT_TYPE_HID;
		}else {
			return STACK_EVENT_TYPE_INVAILD;
		}
}
/*---- class implement ：CProp *----*/
IMPLEMENT_DYNAMIC(CProp, CPropertyPage)
CProp::CProp(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize)
	:CPropertyPage(nIDTemplate, nIDCaption, dwSize)
{
}
CProp::CProp(LPCTSTR lpszTemplateName, UINT nIDCaption, DWORD dwSize)
	: CPropertyPage(lpszTemplateName, nIDCaption, dwSize)
{
}
CProp::~CProp()
{
}
BOOL CProp::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	//((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}
BEGIN_MESSAGE_MAP(CProp, CPropertyPage)
	//HFP
	ON_BN_CLICKED(IDC_HFP_BTN_CONN, &CProp::OnHfpBnClickedBtnConn)
	ON_BN_CLICKED(IDC_HFP_BTN_DISCONN, &CProp::OnHfpBnClickedBtnDisConn)
	ON_BN_CLICKED(IDC_HFP_BTN_ANSWER, &CProp::OnHfpBnClickedBtnAnswer)
	ON_BN_CLICKED(IDC_HFP_BTN_REJECT, &CProp::OnHfpBnClickedBtnReject)
	ON_BN_CLICKED(IDC_HFP_BTN_HANGUP, &CProp::OnHfpBnClickedBtnHangup)
	ON_BN_CLICKED(IDC_HPF_BTN_DIAL, &CProp::OnHfpBnClickedBtnDial)
	//A2DP
	ON_BN_CLICKED(IDC_A2DP_BTN_CONN, &CProp::OnA2dpBnClickedBtnConn)
	ON_BN_CLICKED(IDC_A2DP_BTN_DISCONN, &CProp::OnA2dpBnClickedBtnDisConn)
	//AVRCP
	ON_BN_CLICKED(IDC_AVRCP_BTN_CONN, &CProp::OnAvrcpBnClickedBtnConn)
	ON_BN_CLICKED(IDC_AVRCP_BTN_DISCONN, &CProp::OnAvrcpBnClickedBtnDisConn)
	ON_BN_CLICKED(IDC_AVRCP_BTN_NEXT, &CProp::OnAvrcpBnClickedBtnNext)
	ON_BN_CLICKED(IDC_AVRCP_BTN_PREV, &CProp::OnAvrcpBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_AVRCP_BTN_PLAY, &CProp::OnAvrcpBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_AVRCP_BTN_PAUSE, &CProp::OnAvrcpBnClickedBtnPause)
	ON_BN_CLICKED(IDC_AVRCP_BTN_STOP, &CProp::OnAvrcpBnClickedBtnStop)
	//GATT
	ON_BN_CLICKED(IDC_GATT_BTN_CONN, &CProp::OnGattBnClickedBtnConn)
	ON_BN_CLICKED(IDC_GATT_BTN_DISCONN, &CProp::OnGattBnClickedBtnDisConn)
	ON_BN_CLICKED(IDC_GATT_BTN_SEND, &CProp::OnGattBnClickedBtnSend)

	//SPP
	ON_BN_CLICKED(IDC_SPP_BTN_CONN, &CProp::OnSppBnClickedBtnConn)
	ON_BN_CLICKED(IDC_SPP_BTN_DISCONN, &CProp::OnSppBnClickedBtnDisConn)
	ON_BN_CLICKED(IDC_SPP_BTN_SEND, &CProp::OnSppBnClickedBtnSend)


END_MESSAGE_MAP()

/*-----------hfp msg handler---------*/
void CProp::OnHfpBnClickedBtnConn()
{
	// TODO: 在此添加控件通知处理程序代码
	//GetDlgItem(IDC_HFP_BTN_CONN)->EnableWindow(FALSE);
	uint8_t dut_dev_addr[6] = {0x00, 0x15,0x83, 0x12, 0x34, 0x56};
	hfp_hf_cmd_control('a', dut_dev_addr);
}
void CProp::OnHfpBnClickedBtnDisConn()
{
	hfp_hf_cmd_control('A',NULL);
	//hfp_status_display(0);
}
void CProp::OnHfpBnClickedBtnAnswer()
{
	//GetDlgItem(IDC_HFP_STATIC_DISPLAY)->SetWindowText(_T("CALLING..."));
	//hfp_status_display(4);
	hfp_hf_cmd_control('f', NULL);
}
void CProp::OnHfpBnClickedBtnReject()
{
	//GetDlgItem(IDC_HFP_STATIC_DISPLAY)->SetWindowText(_T("IDLE"));
	//hfp_status_display(1);
	hfp_hf_cmd_control('G', NULL);
}
void CProp::OnHfpBnClickedBtnHangup()
{
	//GetDlgItem(IDC_HFP_STATIC_DISPLAY)->SetWindowText(_T("IDLE"));
	//hfp_status_display(1);
	hfp_hf_cmd_control('F', NULL);
}
void CProp::OnHfpBnClickedBtnDial()
{
	CString t_num;
	char num[20];

	GetDlgItem(IDC_HFP_EDIT_PHONE_NUM)->GetWindowText(t_num);
	string_to_str(num, t_num);

	hfp_hf_cmd_control('i', num);
	app_display(APP_EVT_HFP_HF_OUTGOING_CALL,NULL,0);
	//GetDlgItem(IDC_HFP_STATIC_DISPLAY)->SetWindowText(_T("OUTGOING"));
}
/*-----------hfp msg handler end---------*/

/*-----------a2dp msg handler---------*/
void CProp::OnA2dpBnClickedBtnConn()
{
}
void CProp::OnA2dpBnClickedBtnDisConn()
{
}
/*-----------a2dp msg handler end---------*/

/*-----------avrcp msg handler---------*/
void CProp::OnAvrcpBnClickedBtnConn()
{
}
void CProp::OnAvrcpBnClickedBtnDisConn()
{
}
void CProp::OnAvrcpBnClickedBtnNext()
{
	audio_sink_cmd_control('i', NULL);
}
void CProp::OnAvrcpBnClickedBtnPrev()
{
	audio_sink_cmd_control('I', NULL);
}
void CProp::OnAvrcpBnClickedBtnPlay()
{
	audio_sink_cmd_control('k', NULL);
}
void CProp::OnAvrcpBnClickedBtnPause()
{
	audio_sink_cmd_control('L', NULL);
}
void CProp::OnAvrcpBnClickedBtnStop()
{
	audio_sink_cmd_control('K', NULL);
}
/*-----------avrcp msg handler end---------*/

/*-----------gatt msg handler---------*/
void CProp::OnGattBnClickedBtnConn()
{
}
void CProp::OnGattBnClickedBtnDisConn()
{
}
void CProp::OnGattBnClickedBtnSend()
{
	CString t_data;
	char data[500];
	uint16_t datalen;

	memset(data, 0, sizeof(data));
	GetDlgItem(IDC_GATT_EDIT_SEND)->GetWindowText(t_data);
	datalen = t_data.GetLength();
	//data = (char *)malloc(datalen);
	string_to_str(data, t_data);

	le_streamer_send_data((uint8_t *)data, datalen);
	//free(data);
}
/*-----------gatt msg handler end---------*/

/*-----------spp msg handler---------*/
void CProp::OnSppBnClickedBtnConn()
{
}
void CProp::OnSppBnClickedBtnDisConn()
{
}
void CProp::OnSppBnClickedBtnSend()
{
	CString t_data;
	char data[500];
	uint16_t datalen;

	memset(data, 0, sizeof(data));
	GetDlgItem(IDC_SPP_EDIT_SEND)->GetWindowText(t_data);
	datalen = t_data.GetLength();
	//data = (char *)malloc(datalen);
	string_to_str(data, t_data);

	spp_streamer_send_data((uint8_t *)data, datalen);

	//free(data);
}
/*-----------spp msg handler end---------*/

/*---- class implement ：CPropSheet ----*/
CPropSheet::CPropSheet(UINT num, UINT *id, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	this->m_psh.dwFlags |= PSH_NOAPPLYNOW;
	this->m_psh.dwFlags &= ~(PSH_HASHELP);

	PropPageNum = num;
	m_prop = new CProp *[PropPageNum];
	for (int i = 0; i < PropPageNum; i++) {
		m_prop[i] = new CProp(id[i], 0, 56UL);
		m_prop[i]->m_psp.dwFlags &= ~(PSP_HASHELP);
		AddPage(m_prop[i]);
	}
}

CPropSheet::CPropSheet(UINT num, UINT *id, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	this->m_psh.dwFlags |= PSH_NOAPPLYNOW;
	this->m_psh.dwFlags &= ~(PSH_HASHELP);

	PropPageNum = num; 
	m_prop = new CProp *[PropPageNum];
	for (int i = 0; i < PropPageNum; i++) {
		m_prop[i] = new CProp(id[i], 0, 56UL);
		m_prop[i]->m_psp.dwFlags &= ~(PSP_HASHELP);
		AddPage(m_prop[i]);
	}
}

CPropSheet::~CPropSheet()
{
	for (int i = 0; i < PropPageNum; i++) {
		delete m_prop[i];
	}
	delete m_prop;
}
/*---- class implement ：CPropSheet end----*/

void app_display(uint16_t evt, uint8_t *param, uint16_t len)
{
	CProp *pProp = NULL;
	uint8_t type = get_type_by_event(evt);
	UINT prop_id = get_prop_id_by_type(type);
	pProp = get_pclass_by_prop_pageid(prop_id);

	switch (prop_id) {
		case IDD_PROP_HFP:
		{
			CWnd *hfp_wnd = pProp->GetDlgItem(IDC_HFP_STATIC_DISPLAY);
			if (NULL != hfp_wnd) {
				hfp_wnd->SetWindowText(stack_display_evt_str_map[evt]);
			}
		}
		break;
		case IDD_PROP_A2DP:
		{
			CWnd *a2dp_wnd = pProp->GetDlgItem(IDC_A2DP_STATIC_DISPLAY);
			if (NULL != a2dp_wnd) {
				a2dp_wnd->SetWindowText(stack_display_evt_str_map[evt]);
			}
		}
		break;
		case IDD_PROP_AVRCP:
		{
			CWnd *avrcp_wnd = pProp->GetDlgItem(IDC_AVRCP_STATIC_DISPLAY);
			if (NULL != avrcp_wnd) {
				avrcp_wnd->SetWindowText(stack_display_evt_str_map[evt]);
			}
		}
		break;
		case IDD_PROP_SPP:
		{
			if(APP_EVT_SPP_SER_DATA_RCV != evt){
				CWnd *spp_wnd = pProp->GetDlgItem(IDC_SPP_STATIC_DISPLAY);
				if (NULL != spp_wnd) {
					spp_wnd->SetWindowText(stack_display_evt_str_map[evt]);
				}
			} else {
				wchar_t s_str[1000];
				str_to_wchar_t_str((char*)param, len, s_str, sizeof(s_str));

				CWnd *spp_wnd = pProp->GetDlgItem(IDC_SPP_EDIT_RCV);
				if (NULL != spp_wnd) {
					spp_wnd->SetWindowText(s_str);
				}
			}
		}
		break;
		case IDD_PROP_GATT:
		{
			if (APP_EVT_GATT_SER_DATA_RCV != evt) {
				CWnd *gatt_wnd = pProp->GetDlgItem(IDC_GATT_STATIC_DISPLAY);
				if (NULL != gatt_wnd) {
					gatt_wnd->SetWindowText(stack_display_evt_str_map[evt]);
				}
			} else {
				wchar_t s_str[1000];
				str_to_wchar_t_str((char*)param, len, s_str, sizeof(s_str));

				CWnd *gatt_wnd = pProp->GetDlgItem(IDC_GATT_EDIT_RCV);
				if (NULL != gatt_wnd) {
					gatt_wnd->SetWindowText(s_str);
				}
			}
		}
		break;
		case IDD_PROP_HID:
		{
			/*CWnd *hfp_wnd = pProp->GetDlgItem(IDC_HFP_STATIC_DISPLAY);
			if (NULL != hfp_wnd) {
				hfp_wnd->SetWindowText(hfp_display_str[event_id]);
			}*/
		}
		break;
	}
}

DWORD WINAPI draw_all_prop_page(void *pParam)
{
	int page_num = pSheet->GetPageCount();

	Sleep(10);//等待sheet.DoModal();完成

	for (int i = 0; i < page_num; i++) {
		pSheet->SetActivePage(i);
	}
	pSheet->SetActivePage(0);

	OutputDebugString(_T("draw_all_page exit\n"));
	printf("--->PropSheet.cpp Thread draw_all_page exit\n");

	return 0;
}
/*---- PropSheet Init ----*/
void PropSheetInit(UINT page_num, UINT *page_id)
{
	HANDLE hThread;
	DWORD dwThreadId;
	CPropSheet sheet(page_num, page_id, _T("profile test"));
	pSheet = &sheet;
	pPropPageIdlen = page_num;
	pPropPageId = page_id;
	btstack_evt_display_handler_regeister(app_display);
	//sheet.SetWizardMode();

	//调用线程把每个属性页都画出来，防止设置某页控件属性时死机
	hThread = CreateThread(NULL	// 默认安全属性
		, NULL		// 默认堆栈大小
		, draw_all_prop_page // 线程入口地址
		, NULL	//传递给线程函数的参数
		, 0		// 指定线程立即运行
		, &dwThreadId	//线程ID号
	);
	CloseHandle(hThread);	//关闭线程句柄，内核引用计数减一

	sheet.DoModal();//创建模式对话框

}
