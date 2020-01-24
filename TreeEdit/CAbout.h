// CAbout.h : Declaration of the CCAbout

#ifndef __CABOUT_H_
#define __CABOUT_H_

#include "resource.h"       // main symbols
#include <atlhost.h>

/////////////////////////////////////////////////////////////////////////////
// CCAbout
class CCAbout : 
	public CAxDialogImpl<CCAbout>
{
public:
	char m_NameMenu[255];
public:
	CCAbout()
	{
	}

	~CCAbout()
	{
	}

	enum { IDD = IDD_CABOUT };

BEGIN_MSG_MAP(CCAbout)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_HANDLER(IDC_EDIT_CURRENT_NAME, EN_CHANGE, OnChangeEdit_current_name)
	COMMAND_HANDLER(IDC_BUTTON_INTERNET, BN_CLICKED, OnClickedButton_internet)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	::SetWindowText(GetDlgItem(IDC_EDIT_CURRENT_NAME),m_NameMenu);
		return 1;  // Let the system set the focus
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnChangeEdit_current_name(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		::GetWindowText(GetDlgItem(IDC_EDIT_CURRENT_NAME),m_NameMenu,255);
		return 0;
	}
	LRESULT OnClickedButton_internet(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
      ShellExecute(NULL, "open", "http://www.deltacom.co.il/nv/index.html", NULL, NULL, SW_SHOWNORMAL);
		return 0;
	}
};

#endif //__CABOUT_H_
