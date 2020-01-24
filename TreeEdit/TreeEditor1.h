// TreeEditor1.h : Declaration of the CTreeEditor

#ifndef __TREEEDITOR_H_
#define __TREEEDITOR_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
	#define TCX_ICON  16 
	#define TCY_ICON  16 
	#define TNUM_ICONS 4 


enum TYPESAVE             // type save and export file type
{
   Type_SAVE,
   Type_DHTML,
   Type_XML
} ;               



/////////////////////////////////////////////////////////////////////////////
// CTreeEditor
class CTreeEditor : 
	public CAxDialogImpl<CTreeEditor>
{
private:
    VOID ExpandAllW(DWORD Key);
    void SaveEnd (char *pFileName);
    void SaveStart (char *pFileName);
    void SaveFolderItem(HTREEITEM retNew,char *pFileName);
	void GetItemAttr(HTREEITEM item,char *strName,char *strUrl);
    void SaveItem (HTREEITEM retNew,char *pFileName);
    void SaveChidren (HTREEITEM retNew,char *pFileName);
    HWND hWndEditBoxName;
	HWND hWndEditBoxURL;
    void SaveTreeToFile(char *pFileName);
    HTREEITEM InsertNewTopItem(LPSTR lpszItem, int IndexFolder);
    HTREEITEM InsertNewTopFolder(LPSTR lpszItem, int IndexFolder);
    HTREEITEM InsertCopyTreeMain(HTREEITEM hSrc);
	HTREEITEM InsertCopy(HTREEITEM hSrc);
    HTREEITEM InsertNewItem(LPSTR lpszItem, int IndexFolder);
    HTREEITEM CopyTreeMain(HTREEITEM hDest, HTREEITEM hSrc);
	BOOL TestNoChildren(HTREEITEM hti);
	BOOL TestNoExpandChildren(HTREEITEM hti);
	BOOL TestNoExpandAll(HTREEITEM hti);
    BOOL TestNoExpaned(HTREEITEM hti);
	void TestChildFolderIcon(HTREEITEM hti);
	VOID TestFolderIcon(HTREEITEM hti);
	VOID SaveRestoreExpand(BOOL SaveFlad);
	VOID ResizeWindowPanel();
	BOOL TestFolder(HTREEITEM hDest);
	VOID CopyChildren(HTREEITEM hDest, HTREEITEM hSrc);
	HTREEITEM CopyTree(HTREEITEM hDest, HTREEITEM hSrc);
	HTREEITEM MoveTree(HTREEITEM hDest, HTREEITEM hSrc);
	VOID AddIconsToImageList(DWORD IDI_icon);
		HIMAGELIST hImageList;
		char BufText[255];
		int m_FolderSteck[255];
		int m_LenSteckFolder;
		int m_NewFolderIndex;
		HWND  m_hwndEditCtrl;
		HIMAGELIST m_pImageList;
		long mi_nOpen;
		long mi_nClose;
		long mi_Document;
		HCURSOR StandardCursor;
		static int Lavel;
		HCURSOR NoCursor;
		HCURSOR YesCursor;
		long mi_RootPersonalLink;
		TYPESAVE m_TypeSave;
		HTREEITEM m_DropItem;
		HTREEITEM m_CurrentItem;
		HTREEITEM m_EditTreeItem;
		HTREEITEM m_TargetItem;
		BOOL m_bDragging;
		BOOL m_bDragFolder;
		HWND h_tree;
		int InnerWidth;
		int InnerHeight;
		BOOL boolClearAll;
		HTREEITEM SteckItem[20];
		int LenStectItem;
		char BufEdit[255];
public:
		char m_NameMenu[255];
private:
	VOID SetNameMenu();

	BOOL m_BoolSaveFlag;
	BOOL m_SaveCurrentFolder;
	void WebEncode(char *strSrc);

    char CurrentFileName[255];
	void MSelect(HTREEITEM hti);
	void CaseMenuPopup(DWORD uMenuID);
	HTREEITEM DeleteItem(HTREEITEM hti);
	HTREEITEM InsertNewFolder(LPSTR lpszItem, int IndexFolder);
	BOOL TestExpandAll(HTREEITEM hti);
	VOID ExpandChildren(HTREEITEM hti,DWORD Key);
	VOID ExpandAll(HTREEITEM hti,DWORD Key);
	DWORD GetItemID(HTREEITEM hti,char * Text,int &folder);
	void InsertEndFolder(void){ LenStectItem--;};
	void InsertItem(LPSTR lpstrName, LPSTR IndexFolder);
	void InsertNextFolder(LPSTR lpstrName,int IndexFolder);
	void InsertTopLavel(LPSTR lpstritem,int IndexFolder);
public:
	void LoadTreeFromFile(char * FileName);

	CTreeEditor()
	{

		m_BoolSaveFlag = FALSE;
		boolClearAll = TRUE;
		m_SaveCurrentFolder = FALSE;
		m_EditTreeItem = NULL;
        ::SetWindowText(hWndEditBoxName,"");
		strcpy(CurrentFileName,"Untitles.mnu");
		strcpy(m_NameMenu,"Untitles");
		m_bDragging = FALSE;
        hImageList =  ImageList_Create(TCX_ICON, TCY_ICON, ILC_MASK, TNUM_ICONS, 0); 
		mi_nClose = 0 ;
		AddIconsToImageList(IDI_ICON_CLOSE);
		mi_nOpen = 1 ;
		AddIconsToImageList(IDI_ICON_OPEN);
		mi_Document = 2;
		AddIconsToImageList(IDI_ICON_LINK);
		mi_RootPersonalLink = 3;
		AddIconsToImageList(IDI_ICON_PERSONAL_LINK);



		   

	}





	~CTreeEditor()
	{
		   		ImageList_Destroy(hImageList);
	}

	enum { IDD = IDD_TREEEDITOR };

BEGIN_MSG_MAP(CTreeEditor)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	NOTIFY_HANDLER(IDC_TREE_CTRL, TVN_ITEMEXPANDED, OnItemexpandedTree_ctrl)
	NOTIFY_HANDLER(IDC_TREE_CTRL, TVN_BEGINDRAG, OnBegindragTree_ctrl)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUP)
	MESSAGE_HANDLER(WM_SIZE, OnSize) // ++
	MESSAGE_HANDLER(WM_CLOSE, OnClose)
	NOTIFY_HANDLER(IDC_TREE_CTRL, NM_RCLICK, OnRclickTree_ctrl)
	NOTIFY_HANDLER(IDC_TREE_CTRL, TVN_SELCHANGED, OnSelchangedTree_ctrl)
	COMMAND_HANDLER(IDC_EDIT_NAME, EN_CHANGE, OnChangeEdit_name)
	NOTIFY_HANDLER(IDC_TREE_CTRL, NM_SETFOCUS, OnSetfocusTree_ctrl)
	NOTIFY_HANDLER(IDC_TREE_CTRL, TVN_DELETEITEM, OnDeleteitemTree_ctrl)
	COMMAND_HANDLER(IDC_EDIT_URL, EN_CHANGE, OnChangeEdit_url)
	MESSAGE_HANDLER(WM_COMMAND, OnMenuCommand)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

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

	LRESULT OnItemexpandedTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnBegindragTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		InnerWidth = LOWORD(lParam);
		InnerHeight = HIWORD(lParam);

        ResizeWindowPanel();
		return 0;
	}
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
        SaveRestoreExpand(TRUE);
			EndDialog(0);
   		return 1;
	}

	LRESULT OnRclickTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnSelchangedTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);




	LRESULT OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChangeEdit_name(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSetfocusTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
				 if (m_EditTreeItem == NULL){
					::SetWindowText(hWndEditBoxName,"");
				 }
		// TODO : Add Code for control notification handler.
		return 0;
	}
	LRESULT OnDeleteitemTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnChangeEdit_url(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif //__TREEEDITOR_H_
