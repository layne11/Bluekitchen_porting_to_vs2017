﻿
// btstack_demoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "btstack_demo.h"
#include "btstack_demoDlg.h"
#include "afxdialogex.h"
#include "PropSheet.h"
#include "app_audio.h"

//#include <string>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "app.h"
#ifdef __cplusplus
}
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
static CListCtrl m_list_dev;
UINT PropPage_id[6] = { IDD_PROP_HFP, IDD_PROP_A2DP, IDD_PROP_AVRCP, IDD_PROP_SPP,IDD_PROP_GATT, IDD_PROP_HID };
BOOL is_scaning = FALSE;
static struct device_info g_devs_list[200];
uint16_t g_devs_list_count = 0;
int g_scan_type = 0;



void InitConsoleWindows()
{
	AllocConsole();

#if _MSC_VER <= 1200 //这个是vc6.0
	freopen("CONOUT$", "w+t", stdout);
#else //这个是vc2003以上
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "wt", stdout);
#endif // _MSC_VER > 1000
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CbtstackdemoDlg 对话框



CbtstackdemoDlg::CbtstackdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BTSTACK_DEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbtstackdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEV_LIST, m_list_dev);
}

BEGIN_MESSAGE_MAP(CbtstackdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CbtstackdemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CbtstackdemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_OR_STOP, &CbtstackdemoDlg::OnBnClickedButtonScanOrStop)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEV_LIST, &CbtstackdemoDlg::OnClickListDevice)
	ON_BN_CLICKED(IDC_PROFILE_TEST, &CbtstackdemoDlg::OnBnClickedProfileTest)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CbtstackdemoDlg::OnBnClickedConn)
	ON_BN_CLICKED(IDC_BUTTON_DISCONN, &CbtstackdemoDlg::OnBnClickedDisconn)
	
	ON_BN_CLICKED(IDC_RADIO_CLASS, &CbtstackdemoDlg::OnBnClickedRadioClass)
	ON_BN_CLICKED(IDC_RADIO_BLE, &CbtstackdemoDlg::OnBnClickedRadioBle)
END_MESSAGE_MAP()


// CbtstackdemoDlg 消息处理程序

BOOL CbtstackdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//AllocConsole();
	//freopen("CONOUT$", "w+t", stdout);
	InitConsoleWindows();
	printf("test btstack start.....\n");
	
	currSelect = -1;
	DWORD dwStyle = m_list_dev.GetExtendedStyle();                    //添加列表框的网格线！！！
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_list_dev.SetExtendedStyle(dwStyle);
	m_list_dev.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	m_list_dev.InsertColumn(1, _T("Name"), LVCFMT_CENTER, 300);              //添加列标题！！！！  这里的80,40,90用以设置列的宽度。！！！LVCFMT_LEFT用来设置对齐方式！！！
	m_list_dev.InsertColumn(2, _T("Mac"), LVCFMT_CENTER, 250);
	m_list_dev.InsertColumn(3, _T("type"), LVCFMT_CENTER, 125);
	//m_list_dev.InsertColumn(3, _T("class"), LVCFMT_CENTER, 130);

	((CButton *)GetDlgItem(IDC_RADIO_CLASS))->SetCheck(TRUE);

	app_audio_init();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbtstackdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbtstackdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CbtstackdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbtstackdemoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CbtstackdemoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void update_dev_list(int num, struct device_info *dev)
{
	CString numStr;
	CString devName;
	CString devMac;
	CString devclass;
	CString devtype;
	int item_count = m_list_dev.GetItemCount();
	// get item count
	/*for (int i = 0; i < m_list_dev.GetItemCount(); i++) {
		CString numGet;
		CString numInsert;

		numGet = m_list_dev.GetItemText(i, 0);
		numInsert.Format(_T("%02d"), num);

		if (_ttoi(numGet) == num)
			return;
	}*/

	for (int j = 0; j < g_devs_list_count; j++) {
		if (bd_addr_cmp(dev->bd_addr_t, g_devs_list[j].bd_addr_t) == 0) {
			devName = dev->dev_name;
			//devName.Format(_T("%s"), dev->dev_name);
			m_list_dev.SetItemText(j, 1, devName);
			return;
		}
	}

	num = item_count;
	memcpy(&g_devs_list[g_devs_list_count], dev, sizeof(struct device_info));
	g_devs_list_count++;

	numStr.Format(_T("%02d"), num);
	//devName.Format(_T("%s"),dev->dev_name);
	devName = dev->dev_name;
	devMac.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"), dev->bd_addr_t[0],
		dev->bd_addr_t[1],
		dev->bd_addr_t[2],
		dev->bd_addr_t[3],
		dev->bd_addr_t[4],
		dev->bd_addr_t[5]);
	if (!dev->dev_type)
		devtype.Format(_T("class"));
	else
		devtype.Format(_T("ble"));
	/*devclass.Format(_T("%02x:%02x:%02x"), dev->dev_class[0],
		dev->dev_class[1],
		dev->dev_class[2]);*/

	m_list_dev.InsertItem(num, numStr);
	m_list_dev.SetItemText(num, 1, devName);
	m_list_dev.SetItemText(num, 2, devMac);
	m_list_dev.SetItemText(num, 3, devtype);
	//m_list_dev.SetItemText(num, 3, devclass);
}

void CbtstackdemoDlg::OnBnClickedButtonScanOrStop()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef APP_TYPE_CENTRAL
	le_client_btstack_display_scan_resault_regeister(update_dev_list);
	gap_inquiry_display_result_regeister(update_dev_list);
	if (is_scaning) {
		is_scaning = FALSE;
		GetDlgItem(IDC_BUTTON_SCAN_OR_STOP)->SetWindowText(_T("scan"));
		if (g_scan_type){
			le_streamer_client_scan_set(0);
		} else {
			app_gap_inquiry_scan_set(0);
		}
	}
	else {
		is_scaning = TRUE;
		m_list_dev.DeleteAllItems();
		g_devs_list_count = 0;
		memset(&g_devs_list, 0, sizeof(struct device_info) * 200);
		GetDlgItem(IDC_BUTTON_SCAN_OR_STOP)->SetWindowText(_T("stop"));
		if (g_scan_type) {
			le_streamer_client_scan_set(1);
		} else{
			app_gap_inquiry_scan_set(1);
		}
	}
#endif
}


void CbtstackdemoDlg::OnClickListDevice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos;
	int index;
	CString numGet;

	pos = m_list_dev.GetFirstSelectedItemPosition();
	index = m_list_dev.GetNextSelectedItem(pos);

	numGet = m_list_dev.GetItemText(index, 0);
	currSelect = _ttoi(numGet);

	*pResult = 0;
}


void CbtstackdemoDlg::OnBnClickedProfileTest()
{
	// TODO: 在此添加控件通知处理程序代码

	PropSheetInit(sizeof(PropPage_id) /sizeof(UINT), PropPage_id);
}

void CbtstackdemoDlg::OnBnClickedConn()
{
#ifdef APP_TYPE_CENTRAL
	// TODO: 在此添加控件通知处理程序代码
	if (-1 == currSelect)
		return;

	if (is_scaning) {
		is_scaning = FALSE;
		GetDlgItem(IDC_BUTTON_SCAN_OR_STOP)->SetWindowText(_T("scan"));
		le_streamer_client_scan_set(0);
		app_gap_inquiry_scan_set(0);
	}

	if (!g_devs_list[currSelect].dev_type) {
		//hfp_ag_cmd_control('a',g_devs_list[currSelect].bd_addr_t);
		//Sleep(200);
		audio_source_cmd_control('b', g_devs_list[currSelect].bd_addr_t);
		Sleep(200);
		audio_source_cmd_control('c', g_devs_list[currSelect].bd_addr_t);
		//Sleep(200);
		//spp_client_conn_disconn(1, g_devs_list[currSelect].bd_addr_t);
	} else {
		le_client_conn_disconn(1,g_devs_list[currSelect].bd_addr_t, g_devs_list[currSelect].dev_addr_type);
	}
#endif
}

void CbtstackdemoDlg::OnBnClickedDisconn()
{
#ifdef APP_TYPE_CENTRAL
	// TODO: 在此添加控件通知处理程序代码
		hfp_ag_cmd_control('A', NULL);
		audio_source_cmd_control('B', NULL);
		audio_source_cmd_control('C', NULL);
		spp_client_conn_disconn(0, NULL);
		le_client_conn_disconn(0, NULL, 0);
#endif

#ifdef APP_TYPE_PERIPHERAL
		hfp_hf_cmd_control('A', NULL);
#endif
}

void CbtstackdemoDlg::OnBnClickedRadioClass()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton *)GetDlgItem(IDC_RADIO_CLASS))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO_BLE))->SetCheck(FALSE);
	g_scan_type = 0;
}


void CbtstackdemoDlg::OnBnClickedRadioBle()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton *)GetDlgItem(IDC_RADIO_CLASS))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_BLE))->SetCheck(TRUE);
	g_scan_type = 1;

}
