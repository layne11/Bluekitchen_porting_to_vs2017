
// btstack_demoDlg.h: 头文件
//

#pragma once


// CbtstackdemoDlg 对话框
class CbtstackdemoDlg : public CDialogEx
{
// 构造
public:
	CbtstackdemoDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BTSTACK_DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	int currSelect;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonScanOrStop();
	afx_msg void OnClickListDevice(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedProfileTest();
	afx_msg void OnBnClickedConn();
	afx_msg void OnBnClickedDisconn();
	afx_msg void OnBnClickedRadioClass();
	afx_msg void OnBnClickedRadioBle();
};
extern int main(int argc, const char * argv[]);
