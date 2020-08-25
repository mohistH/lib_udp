
// mfc_demoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_demo.h"
#include "mfc_demoDlg.h"
#include "afxdialogex.h"





#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// Cmfc_demoDlg 对话框



Cmfc_demoDlg::Cmfc_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_DEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef _use_my_udp_
#undef new
	_pudp = std::unique_ptr<my_udp>(new(std::nothrow) my_udp);
#endif // 
}

void Cmfc_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cmfc_demoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Cmfc_demoDlg 消息处理程序

BOOL Cmfc_demoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

#ifdef _use_my_udp_
	// created failure
	if (!_pudp)
	{
		AfxMessageBox(L"udp创建失败");
		return TRUE;
	}


	//return TRUE;



	_timer = SetTimer(10086, 40, NULL);

	int index = 0;
	for (; index < 16; )
		_arr_send[index++] = 49 + index;

	udp_param param;

	// log file
	param._is_log_debug = true;
	// udp type
	param._cast_type = udp_multi_cast;

	// the port of destination
	param._port_dst = 12345;

	// Whether to receive messages from yourself
	param._recv_loop = true;

	// local IP4
	char arr[] = "10.1.1.5";
	// Target IP
	char arr_dst[] = "233.0.0.11";

	memcpy(param._pip4_dst, arr_dst, strlen(arr_dst));
	memcpy(param._pip4_local, arr, strlen(arr));


	// 初始化参数
	int ret_val = _pudp->init_ip4(param);
	if (0 != ret_val)
	{
		AfxMessageBox(L"初始化失败");
		return TRUE;
	}

	// 打开UDP
	ret_val = _pudp->open(10);
	if (0 != ret_val)
	{
		AfxMessageBox(L"OPEN失败");
		CString str;
		str.Format(L"%d", ret_val);
		AfxMessageBox(str);
		return TRUE;
	}

	
	_is_open = true;


#endif // 

	// -------------------------------------------------------------------------------
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cmfc_demoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cmfc_demoDlg::OnPaint()
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
HCURSOR Cmfc_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
*	@brief:
*/
void Cmfc_demoDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	static int index = 1;
	static int ret = 0;

	if (!_is_open)
		return;

#ifdef _use_my_udp_
	if (_pudp)
	{
		//ret = _pudp->send(_arr_send, 16);
		//TRACE("\n\n 第%d发送，发送结果：%d\n\n", index++, ret);
	}
#endif // _use_my_udp_
}



void Cmfc_demoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	_is_open = false;

	if (0 != _timer)
	{
		KillTimer(_timer);
		_timer = 0;
	}



	// TODO: 在此处添加消息处理程序代码
#ifdef _use_my_udp_

	if (_pudp)
		_pudp->shutdown();
	
#endif // !


	

}
