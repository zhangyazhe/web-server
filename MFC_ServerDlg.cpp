
// MFC_ServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_Server.h"
#include "MFC_ServerDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <Winsock2.h>
#include <thread>
#include <conio.h> 
#include <regex>
#include <string>
#include <fstream>
#include <string.h>
#include <tchar.h> 
#pragma comment(lib,"ws2_32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//用户输入的ip地址
BYTE f0, f1, f2, f3;
CString ip;
SOCKET servSock;//监听套接字全局变量
BOOL g_Allow;
int format = 1;//用于规范格式
int format2 = 0;//用于规范格式


std::string localPath("\\\\vmware-host\\Shared Folders\\Mac 上的 zhangyazhe\\MyFiles\\大三上\\计算机网络\\Network_Lab\\lab1_Socket\\source");//默认服务器资源路径

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


// CMFCServerDlg 对话框



CMFCServerDlg::CMFCServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_display);
	DDX_Control(pDX, IDC_BUTTON1, m_button1);
	DDX_Control(pDX, IDC_EDIT2, m_portnum);
}

BEGIN_MESSAGE_MAP(CMFCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCServerDlg::OnBnClickedButton1)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CMFCServerDlg::OnIpnFieldchangedIpaddress1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCServerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCServerDlg 消息处理程序

BOOL CMFCServerDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCServerDlg::OnPaint()
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
HCURSOR CMFCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCServerDlg::OnBnClickedButton1()//开始监听按钮
{
	g_Allow = TRUE;
	//点击连接后禁用按钮
	HWND hConnBtn = ::GetDlgItem(GetSafeHwnd(), IDC_BUTTON1);
	if (hConnBtn) {
		::EnableWindow(hConnBtn, FALSE);
	}
	HWND hConnBtn2 = ::GetDlgItem(GetSafeHwnd(), IDC_BUTTON2);
	if (hConnBtn2) {
		::EnableWindow(hConnBtn2, FALSE);
	}
	CString str = _T("");//用于在文本后面追加文本
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		GetDlgItemText(IDC_EDIT1, str);
		if (format != 1) {
			str += "\r\n";
		}
		str += "Winsock初始化失败";
		format++;
		m_display.SetWindowText(str);
		return;
	}
	GetDlgItemText(IDC_EDIT1, str);
	if (format != 1) {
		str += "\r\n";
	}
	str += "Winsock初始化成功";
	format++;
	m_display.SetWindowText(str);
	if (wsaData.wVersion != 0x0101)
	{
		//版本支持不够
		//报告错误给用户，清除Winsock，返回
		GetDlgItemText(IDC_EDIT1, str);
		str += "\r\n版本错误";
		m_display.SetWindowText(str);
		WSACleanup();
		return;
	}
	GetDlgItemText(IDC_EDIT1, str);
	str += "\r\n版本正确";
	m_display.SetWindowText(str);
	//创建服务器监听套接字
	servSock = socket(AF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET) 
	{
		GetDlgItemText(IDC_EDIT1, str);
		str += "\r\n服务器监听套接字创建失败";
		m_display.SetWindowText(str);
		return;
	}
	GetDlgItemText(IDC_EDIT1, str);
	str += "\r\n服务器监听套接字创建成功";
	m_display.SetWindowText(str);

	//配置端口号，若无用户输入则使用默认端口号
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	int portnum = 80;
	CString str_portnum = _T("");
	GetDlgItemText(IDC_EDIT2, str_portnum);
	std::string sDigit = static_cast<CStringA>(str_portnum);
	portnum = (int)atoi(sDigit.c_str());
	if (portnum == 0) {
		portnum = 80;
	}

	//配置本地资源路径
	CString str_local_url = _T("");
	GetDlgItemText(IDC_EDIT3, str_local_url);
	std::string local_url = static_cast<CStringA>(str_local_url);
	if (local_url != "") {
		localPath = local_url;
	}

	//解析用户输入的ip地址，若无用户输入则使用默认地址
	CIPAddressCtrl* ipCtrl = (CIPAddressCtrl*)this->GetDlgItem(IDC_IPADDRESS1);
	ipCtrl->GetAddress(f0, f1, f2, f3);
	ip.Format(_T("%d.%d.%d.%d"), f0, f1, f2, f3);
	//htons和htonl函数把主机字节顺序转换为网络字节顺序，分别用于//短整型和长整型数据
	addr.sin_port = htons(portnum);
	if (f0 == 0 && f1 == 0 && f2 == 0 && f3 == 0) {
		addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else {
		addr.sin_addr.s_addr = inet_addr(_T(ip));
	}

	// 将服务器套接字与端口号绑定
	int nRc = bind(servSock, (LPSOCKADDR)&addr, sizeof(addr));
	if (nRc == SOCKET_ERROR) {
		nRc = WSAGetLastError();
		GetDlgItemText(IDC_EDIT1, str);
		str += "\r\n绑定出错";
		m_display.SetWindowText(str);
		CString errorstr;
		errorstr.Format(_T("%d"), nRc);
		GetDlgItemText(IDC_EDIT1, str);
		str += "\r\n";
		str += errorstr;
		m_display.SetWindowText(str);
		::EnableWindow(hConnBtn, TRUE);
		return;
	}
	GetDlgItemText(IDC_EDIT1, str);
	str += "\r\n绑定成功";
	m_display.SetWindowText(str);
	//监听客户端等待客户机的连接请求
	if (listen(servSock, 10) == SOCKET_ERROR)
	{
		GetDlgItemText(IDC_EDIT1, str);
		str += "\r\n监听出错";
		m_display.SetWindowText(str);
		return;
	}
	GetDlgItemText(IDC_EDIT1, str);
	str += "\r\n正在监听...";
	m_display.SetWindowText(str);

	AfxBeginThread(ThreadListen, NULL);
	return;
}

//线程:监听，创建监听套接字
UINT ThreadListen(LPVOID pParam) {
	//int i = 0;
	TCHAR text2[1024] = { 0 };
	TCHAR add2[50] = _T(" ");
	int length = sizeof(sockaddr);
	sockaddr_in clientaddr;
	SOCKET* talkSock = new SOCKET;
	HWND hConnBtn2 = ::GetDlgItem(AfxGetMainWnd()->m_hWnd, IDC_BUTTON2);
	HWND hEdit2 = ::GetDlgItem(AfxGetMainWnd()->m_hWnd, IDC_EDIT5);//获得编辑框句柄
	while (g_Allow == TRUE) {
		//if (Terminate == FALSE) {
		*talkSock = accept(servSock, (sockaddr*)&clientaddr, &length);
		if (hConnBtn2 && g_Allow == TRUE) {
			::EnableWindow(hConnBtn2, TRUE);
		}
		if (g_Allow == FALSE) {
			return 0;
		}
		else {
			::GetWindowText(hEdit2, text2, 1024);
			if (format2 == 0) {
				lstrcpy(add2, _T("客户机IP地址："));
				format2 = 1;
			}
			else {
				lstrcpy(add2, _T("\r\n\r\n客户机IP地址："));
			}
			lstrcat(text2, add2);
			TCHAR c[20] = { 0 };
			sprintf_s(c, "%d", (int)clientaddr.sin_addr.S_un.S_un_b.s_b1);//这句将nativeobjcet作为字符串输出到c中
			lstrcpy(add2, c);
			lstrcat(add2, _T("."));
			sprintf_s(c, "%d", (int)clientaddr.sin_addr.S_un.S_un_b.s_b2);
			lstrcat(add2, c);
			lstrcat(add2, _T("."));
			sprintf_s(c, "%d", (int)clientaddr.sin_addr.S_un.S_un_b.s_b3);
			lstrcat(add2, c);
			lstrcat(add2, _T("."));
			sprintf_s(c, "%d", (int)clientaddr.sin_addr.S_un.S_un_b.s_b4);
			lstrcat(add2, c);
			strcpy_s(c, _T("		端口号："));
			strcat_s(add2, c);
			sprintf_s(c, "%d", (int)clientaddr.sin_port);
			strcat_s(add2, c);
			lstrcat(text2, add2);
			::SetWindowText(hEdit2, text2);

			AfxBeginThread(ThreadConn, talkSock);
		}
	}
	closesocket(servSock);
}

//线程:通信
UINT ThreadConn(LPVOID pParam)
{
	if (g_Allow == FALSE) {
		return 0;
	}
	SOCKET* sock = (SOCKET*)pParam;
	SOCKET talksocket = *sock;
	TCHAR text[1024] = { 0 };
	TCHAR add[50] = _T(" ");
	TCHAR text2[1024] = { 0 };
	TCHAR add2[50] = _T(" ");
	char* ptr = NULL;

	int sockerror = 0;
	HWND hEdit1 = ::GetDlgItem(AfxGetMainWnd()->m_hWnd, IDC_EDIT1);//获得编辑框句柄
	HWND hEdit2 = ::GetDlgItem(AfxGetMainWnd()->m_hWnd, IDC_EDIT5);//获得编辑框句柄
	if (talksocket == INVALID_SOCKET)
	{
		::GetWindowText(hEdit1, text, 1024);
		lstrcpy(add, _T("\r\n接受连接请求错误"));
		lstrcat(text, add);
		::SetWindowText(hEdit1, text);
		sockerror = WSAGetLastError();
		return 0;
	}
	else
	{
		::GetWindowText(hEdit1, text, 1024);
		lstrcpy(add, _T("\r\n接受连接请求成功"));
		lstrcat(text, add);
		::SetWindowText(hEdit1, text);

		//解析报文信息
		int sendre = 0;
		char* mime = NULL;//文件类型 
		char* rcvbuf = (char*)malloc(1024 * sizeof(char));
		int rcvlen = 1024;
		int rcvcharnum = 0;
		const char* _404 = "404 Not Found\r\n";
		const char* _400 = "400 Bad Request\r\n";
		rcvcharnum = recv(talksocket, rcvbuf, rcvlen, 0);

		if (rcvcharnum == SOCKET_ERROR)
		{
			::GetWindowText(hEdit1, text, 1024);
			lstrcpy(add, _T("\r\n接收数据失败"));
			lstrcat(text, add);
			::SetWindowText(hEdit1, text);
			free(rcvbuf);
			return 0;
		}
		else { 
			//将请求命令行进行输出
			::GetWindowText(hEdit2, text2, 1024);
			const char split[5] = "\r\n";
			char* head = strtok_s(rcvbuf, split, &ptr);
			strcpy_s(add2, _T("\r\n"));
			strcat_s(add2, head);
			strcat_s(text2, add2);
			::SetWindowText(hEdit2, text2);

			//对接收到的数据进行解析
			std::smatch strmatch;//正则表达式匹配结果文本
			std::regex regulation("([A-Za-z]+) +(.*) +(HTTP/[0-9][.][0-9])");//匹配报文头
			std::string str(rcvbuf);//接收到的原始文本

			int matchnum = std::regex_search(str, strmatch, regulation);
			if (matchnum == 0) {
				::GetWindowText(hEdit1, text, 1024);
				lstrcpy(add, _T("\r\n报文头错误"));
				lstrcat(text, add);
				::SetWindowText(hEdit1, text);
				sendre = send(talksocket, _400, strlen(_400), 0);
				if (sendre == SOCKET_ERROR) {
					::GetWindowText(hEdit1, text, 1024);
					lstrcpy(add, _T("\r\n错误信息发送给客户机时出错"));
					lstrcat(text, add);
					::SetWindowText(hEdit1, text);
				}
				else {
					::GetWindowText(hEdit1, text, 1024);
					lstrcpy(add, _T("\r\n已将错误信息发送给客户机"));
					lstrcat(text, add);
					::SetWindowText(hEdit1, text);
				}
				closesocket(talksocket);
				::GetWindowText(hEdit1, text, 1024);
				lstrcpy(add, _T("\r\n中断连接 继续监听..."));
				lstrcat(text, add);
				::SetWindowText(hEdit1, text);
				free(rcvbuf);
				return 0;
			}
			else {
				std::string msg_get = strmatch[1];//"GET"
				std::string msg_url = strmatch[2];
				std::smatch filetype;
				std::regex regulation2("[.].*");
				matchnum = regex_search(msg_url, filetype, regulation2);
				if (matchnum == 0) {
					::GetWindowText(hEdit1, text, 1024);
					lstrcpy(add, _T("\r\n错误：没有文件类型"));
					lstrcat(text, add);
					::SetWindowText(hEdit1, text);
					sendre = send(talksocket, _404, strlen(_404), 0);
					if (sendre == SOCKET_ERROR) {
						::GetWindowText(hEdit1, text, 1024);
						lstrcpy(add, _T("\r\n错误信息发送给客户机时出错"));
						lstrcat(text, add);
						::SetWindowText(hEdit1, text);
					}
					else {
						::GetWindowText(hEdit1, text, 1024);
						lstrcpy(add, _T("\r\n已将错误信息发送给客户机"));
						lstrcat(text, add);
						::SetWindowText(hEdit1, text);
					}
					closesocket(talksocket);
					::GetWindowText(hEdit1, text, 1024);
					lstrcpy(add, _T("\r\n中断连接 继续监听..."));
					lstrcat(text, add);
					::SetWindowText(hEdit1, text);
					free(rcvbuf);
					return 0;
				}
				else {
					std::ifstream f(localPath + msg_url, std::ios::binary);
					if (!f) {//没有找到文件
						::GetWindowText(hEdit1, text, 1024);
						lstrcpy(add, _T("\r\n没有找到文件"));
						lstrcat(text, add);
						::SetWindowText(hEdit1, text);
						sendre = send(talksocket, _404, strlen(_404), 0);
						if (sendre == SOCKET_ERROR) {
							::GetWindowText(hEdit1, text, 1024);
							lstrcpy(add, _T("\r\n错误信息发送给客户机时出错"));
							lstrcat(text, add);
							::SetWindowText(hEdit1, text);
						}
						else {
							::GetWindowText(hEdit1, text, 1024);
							lstrcpy(add, _T("\r\n已将错误信息发送至客户机"));
							lstrcat(text, add);
							::SetWindowText(hEdit1, text);
						}
						closesocket(talksocket);
						::GetWindowText(hEdit1, text, 1024);
						lstrcpy(add, _T("\r\n中断连接 继续监听..."));
						lstrcat(text, add);
						::SetWindowText(hEdit1, text);
						free(rcvbuf);
						return 0;
					}
					else {//找到了对应的文件 
						char c;
						std::string content = "";
						int f_len = 0;
						while (f.read(&c, sizeof(char))){
							content.push_back(c);
							f_len++;
						}
						const char* data = content.c_str();
						if (filetype[0] == ".txt") {
							sendre = send(talksocket, data, strlen(data), 0);
						}
						else if (filetype[0] == ".jpg")
						{
							const char* head = "HTTP/1.1 200 OK\r\nContent-Type:image/jpg\r\n\r\n";
							int length = strlen(head) + f_len;
							sendre = send(talksocket, head, strlen(head), 0);
							sendre = send(talksocket, data, f_len, 0);
						}
						else if (filetype[0] == ".gif")
						{
							const char* head = "HTTP/1.1 200 OK\r\nContent-Type:image/gif\r\n\r\n";
							int length = strlen(head) + f_len; 
							sendre = send(talksocket, head, strlen(head), 0);
							sendre = send(talksocket, data, f_len, 0);
						}
						else if (filetype[0] == ".html") {
							mime = "text/html";
							char response[10240] = { 0 };
							sprintf_s(response, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n");
							strcat_s(response, data);
							sendre = send(talksocket, response, strlen(response), 0);
						}

						if (sendre == SOCKET_ERROR) {
							::GetWindowText(hEdit1, text, 1024);
							lstrcpy(add, _T("\r\n文件发送给客户机时出错"));
							lstrcat(text, add);
							::SetWindowText(hEdit1, text);
							sendre = send(talksocket, _400, strlen(_400), 0);
							if (sendre == SOCKET_ERROR) {
								::GetWindowText(hEdit1, text, 1024);
								lstrcpy(add, _T("\r\n错误信息发送给客户机时出错"));
								lstrcat(text, add);
								::SetWindowText(hEdit1, text);
							}
							else {
								::GetWindowText(hEdit1, text, 1024);
								lstrcpy(add, _T("\r\n已将错误信息发送给客户机"));
								lstrcat(text, add);
								::SetWindowText(hEdit1, text);
							}
							closesocket(talksocket);
							::GetWindowText(hEdit1, text, 1024);
							lstrcpy(add, _T("\r\n中断连接 继续监听..."));
							lstrcat(text, add);
							::SetWindowText(hEdit1, text);
						}
						else {
							::GetWindowText(hEdit1, text, 1024);
							lstrcpy(add, _T("\r\n已将文件发送给客户机"));
							lstrcat(text, add);
							::SetWindowText(hEdit1, text);
							closesocket(talksocket);
						}
						f.close();
						::GetWindowText(hEdit1, text, 1024);
						lstrcpy(add, _T("\r\n中断连接 继续监听.."));
						lstrcat(text, add);
						::SetWindowText(hEdit1, text);
						free(rcvbuf);
						return 0;
					}
				}
			}
		}
	}
}


void CMFCServerDlg::OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CMFCServerDlg::OnBnClickedButton2()
{
	g_Allow = FALSE;
	HWND hConnBtn1 = ::GetDlgItem(GetSafeHwnd(), IDC_BUTTON1);
	if (hConnBtn1) {
		::EnableWindow(hConnBtn1, TRUE);
	}
	HWND hConnBtn2 = ::GetDlgItem(GetSafeHwnd(), IDC_BUTTON2);
	if (hConnBtn2) {
		::EnableWindow(hConnBtn2, FALSE);
	}
	CString str = _T("");
	GetDlgItemText(IDC_EDIT1, str);
	str += "\r\n断开连接";
	m_display.SetWindowText(str);
}
