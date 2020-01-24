// TreeEditor1.cpp : Implementation of CTreeEditor
#include "stdafx.h"
#include "CAbout.h"
#include "commctrl.h"
#include "TreeEditor1.h"
#include <stdio.h>
#include "windowsx.h"





	LRESULT CTreeEditor::OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
			HTREEITEM h_item = NULL ;
	TVHITTESTINFO tv_ht ;
	TVITEM tv ;
	ZeroMemory(&tv, sizeof(TVITEM)) ;
        ZeroMemory(&tv_ht, sizeof(TVHITTESTINFO)) ;
	
	if(m_bDragging)
	{
	   ImageList_DragLeave(NULL) ;
	   ImageList_EndDrag() ;
	   ReleaseCapture() ;
	   //determin if we let up on an item
	   GetCursorPos(&(tv_ht.pt)) ;
	   ::ScreenToClient(h_tree, &(tv_ht.pt)) ;
           tv_ht.flags = TVHT_ONITEM ;
//	   ShowCursor(TRUE) ;
	   SetCursor(StandardCursor);
	   DestroyCursor(NoCursor);

	   m_bDragging = FALSE ;
	   ImageList_Destroy(m_pImageList);

	   if(m_TargetItem) 
	   {
			HTREEITEM pNewTarget = MoveTree(m_TargetItem,m_DropItem);
//		    TreeView_DeleteItem(h_tree,m_DropItem);
			TreeView_SelectItem(h_tree,pNewTarget);
			TestFolderIcon(pNewTarget);
	   }
	   TreeView_SelectDropTarget(h_tree,NULL);
	}

		return 0;
	}


	LRESULT CTreeEditor::OnBegindragTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
//
//  HWND hwndTV, LPNMTREEVIEW lpnmtv
//

	HWND hwndTV = GetDlgItem(IDC_TREE_CTRL);
	h_tree = hwndTV;
    

	 LPNMTREEVIEW lpnmtv = (NMTREEVIEW *) pnmh; 
 

//	*pResult = 0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*) pnmh;

	//
	// Do nothing if the user is attempting to drag a top-level item.
	//
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
//	if (TreeView_GetParent(hwndTV,hItem) == NULL)
//		return FALSE;

	m_DropItem = hItem;
	m_CurrentItem = m_DropItem;
	if (m_CurrentItem != NULL ){
       MSelect(m_CurrentItem);
	}

	//
	// Create a drag image. If the assertion fails, you probably forgot
	// to assign an image list to the control with SetImageList. Create-
	// DragImage will not work if the control hasn't been assigned an
	// image list!
	//
	m_pImageList = TreeView_CreateDragImage (hwndTV,hItem);
   

	if (m_pImageList != NULL) { // Just to be sure
    ImageList_BeginDrag(m_pImageList, 0, 0, 0) ;
		//
		// Compute the coordinates of the "hot spot"--the location of the
		// cursor relative to the upper left corner of the item rectangle.
		//
		RECT  rect;
		TreeView_GetItemRect(hwndTV,hItem, &rect, TRUE);


   //hide the cursor
   StandardCursor = GetCursor();
    NoCursor = LoadCursor(_Module.GetModuleInstance(),MAKEINTRESOURCE(IDC_CURSOR_NO_DROP_1)); 
	YesCursor = StandardCursor;

//   ShowCursor(FALSE) ;
   //capture the mouse
   ::SetCapture(::GetParent(hwndTV)) ;
   //set global flag to indicate we are in the middle of a drag operation
   POINT point;
	   point.x = rect.left;
      point.y = rect.top;
	  ::ClientToScreen(hwndTV, &point) ;
   //paint our drag image and lock the screen.
   ImageList_DragEnter(NULL, point.x, point.y) ;
   
   m_bDragging = TRUE;
   m_bDragFolder = TestFolder(m_DropItem);
	}
		return 0;
	}




	LRESULT CTreeEditor::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

 
   TVHITTESTINFO tvht;  // hit test information 
	
      
   POINT pnt ;

   HTREEITEM h_item = NULL ;
   	HWND hwndTV = GetDlgItem(IDC_TREE_CTRL);
    long yCur = GET_Y_LPARAM(lParam) ;
   long xCur = GET_X_LPARAM(lParam) ;
   pnt.x = xCur ;
   pnt.y = yCur;
   if(m_bDragging)
   {
      //unlock window and allow updates to occur
      ImageList_DragLeave(NULL) ;
      ::ClientToScreen(hwndTV, &pnt) ;
      //check with the tree control to see if we are on an item
      TVHITTESTINFO tv_ht ;
      ZeroMemory(&tv_ht, sizeof(TVHITTESTINFO)) ;
      tv_ht.flags = TVHT_ONITEM ;
      tv_ht.pt.x = pnt.x ;
      tv_ht.pt.y = pnt.y ;
      ::ScreenToClient(hwndTV, &(tv_ht.pt)) ;
      h_item = (HTREEITEM)SendMessage(hwndTV, TVM_HITTEST, 0, (LPARAM)&tv_ht) ;
	  if (h_item == m_DropItem ) {
					h_item = NULL;
	  }
      if(h_item)
      {  //if we had a hit, then drop highlite the item
		  if (m_bDragFolder == TRUE ){
			HTREEITEM tar = TreeView_GetParent(hwndTV,h_item);
			while(tar != NULL ) {
				if (tar == m_DropItem ){
					h_item = NULL;
					break;
				}
				tar = TreeView_GetParent(hwndTV,tar);
			}
		  } 
      }
	  if ( h_item != NULL ) {
	  	     SendMessage(hwndTV, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)h_item) ;
		     m_TargetItem = h_item;
	  }else {
		     m_TargetItem = NULL;	
	         MSelect(m_CurrentItem);
	  }




      //paint the image in the new location
      ImageList_DragMove(pnt.x,pnt.y) ;
      //lock the screen again
      ImageList_DragEnter(NULL, pnt.x, pnt.y) ;

        // Find out if the pointer is on the item. If it is, 
		// highlight the item as a drop target. 
        tvht.pt.x = xCur; 
        tvht.pt.y = yCur; 

	  if (m_TargetItem){
			  SetCursor(YesCursor);
	  }else {
				   SetCursor(NoCursor);
	  }

//        if ((htiTarget = TreeView_HitTest(hwndTV, &tvht)) != NULL) { 
//            TreeView_SelectDropTarget(hwndTV, htiTarget); 
//       } 

   }

 
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		return 0;
	}








	LRESULT CTreeEditor::OnItemexpandedTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
	HTREEITEM	hti;

	HWND thWnd = h_tree;
    

	 NMTREEVIEW * pnmtv = (NMTREEVIEW *) pnmh; 
 
       hti = (HTREEITEM)(pnmtv->itemNew).hItem; 

		TestFolderIcon(hti);
		TestChildFolderIcon(hti);

		return 0;
	}

void CTreeEditor::GetItemAttr(HTREEITEM item,char *strName,char *strUrl){
	HTREEITEM hItem = item;
//-----

	    TVITEM tvi_Sourse;
		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hItem; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
//-----
		 m_EditTreeItem = hItem;
        char * ParStr = (LPSTR)tvi_Sourse.lParam;
		strcpy(strName,strTemp);
		if (strlen(strTemp) == 0 ){
			strcpy(strName,".");
		}
		strcpy(strUrl,ParStr);
		if (strlen(ParStr) == 0 ){
			strcpy(strUrl,".");
		}

//		::SetWindowText(hWndEditBoxURL,ParStr);
//		::SetWindowText(hWndEditBoxName,strTemp);
	}


LRESULT CTreeEditor::OnSelchangedTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
	 LPNMTREEVIEW lpnmtv = (NMTREEVIEW *) pnmh; 
 

//	*pResult = 0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*) pnmh;

	//
	// Do nothing if the user is attempting to drag a top-level item.
	//
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
         TreeView_SelectDropTarget(h_tree,NULL);
         TreeView_SelectDropTarget(h_tree,pNMTreeView->itemNew.hItem);

//-----

	    TVITEM tvi_Sourse;
		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hItem; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
//-----
		 m_EditTreeItem = hItem;
        char * ParStr = (LPSTR)tvi_Sourse.lParam;
		::SetWindowText(hWndEditBoxURL,ParStr);
		::SetWindowText(hWndEditBoxName,strTemp);
//    HWND hWndEditBoxName;
// 	  HWND hWndEditBoxURL;
		// TODO : Add Code for control notification handler.
		return 0;
	}


	LRESULT CTreeEditor::OnChangeEdit_url(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if (m_EditTreeItem != NULL){
	
	    TVITEM tvi_Sourse;


		char strTemp[255];
		char strTempName[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = m_EditTreeItem; 
        tvi_Sourse.pszText = strTempName; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
//-----
		strTemp[0]='\0';
		strTemp[249]='\0';
		::GetWindowText(hWndEditBoxURL,strTemp,250);
		 
		 free((void *)(tvi_Sourse.lParam));
		 int lenField = strlen(strTemp)+2;
         LPSTR newStr = (LPSTR)malloc(lenField);
		 strcpy(newStr,strTemp);
		 tvi_Sourse.lParam = (LPARAM) newStr;
        TreeView_SetItem(h_tree, &tvi_Sourse); 
		}
		return 0;
	}




LRESULT CTreeEditor::OnChangeEdit_name(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if (m_EditTreeItem != NULL){

	
	    TVITEM tvi_Sourse;


		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = m_EditTreeItem; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
//-----
		::GetWindowText(hWndEditBoxName,strTemp,250);
        TreeView_SetItem(h_tree, &tvi_Sourse); 
		}
		// TODO : Add Code for control notification handler.
		return 0;
	}



VOID CTreeEditor::AddIconsToImageList(DWORD IDI_icon)
{

    HICON hicon;
    hicon = LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_icon)); 
	ImageList_AddIcon(hImageList, hicon); 
}

	LRESULT CTreeEditor::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
    LPTVINSERTSTRUCT pis;
	HWND tmhWnd = GetDlgItem(IDC_TREE_CTRL);
	h_tree = tmhWnd;
	    HICON hIcon = ::LoadIcon(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_ICON_PERSONAL_LINK));
	this->SetIcon(hIcon);

	    TreeView_SetImageList(
       tmhWnd,
       hImageList,
       TVSIL_NORMAL );	




    TreeView_InsertItem(
    tmhWnd,
    &pis
    );		


	RECT rect;
	GetClientRect(&rect);
	InnerWidth = rect.right-rect.left;
	InnerHeight = rect.bottom - rect.top;



        ResizeWindowPanel();
		SaveRestoreExpand(FALSE);
		SetNameMenu();

		return 1;  // Let the system set the focus
	}


	LRESULT CTreeEditor::OnRclickTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
	char Buf[255];
	BOOL folder;
    h_tree = GetDlgItem(IDC_TREE_CTRL);
	HTREEITEM	hti;
	hti = NULL;
	hti = TreeView_GetDropHilight(h_tree);
    if (hti == NULL ){
		hti = TreeView_GetSelection(h_tree);
		  TreeView_SelectDropTarget(h_tree,hti);

	} else {
	  TreeView_SelectItem(h_tree,hti);
	}
	POINT pt;
	/*DWORD par =*/  GetItemID(hti,Buf,folder);


		//=====
         HMENU hmenuMain = NULL;
           HMENU hmenuContext = NULL;
		   hmenuMain = LoadMenu(_Module.GetModuleInstance(), 
		      MAKEINTRESOURCE(IDR_MENU_POPUP_TREE));
			  GetCursorPos(&pt);


			switch( folder )
			{
				// item
			case 0:
				EnableMenuItem(hmenuMain,ID_POPMENUTREE_EXPANDALL,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				EnableMenuItem(hmenuMain,ID_POPMENUTREE_COLLAPSED,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				break;
				// open
			case 1:
				if (!TestNoExpandAll(hti)){
				   EnableMenuItem(hmenuMain,ID_POPMENUTREE_EXPANDALL,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				}
				break;
				// close 
			case 2:
				if (TestNoChildren(hti)){
				   EnableMenuItem(hmenuMain,ID_POPMENUTREE_EXPANDALL,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				}
				EnableMenuItem(hmenuMain,ID_POPMENUTREE_COLLAPSED,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
				break;
			}

		   hmenuContext = GetSubMenu(hmenuMain, 0);

            UINT uMenuID = ::TrackPopupMenu(hmenuContext, 
            TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
            pt.x, pt.y, 0, h_tree, NULL);
			DestroyMenu(hmenuContext);
			DestroyMenu(hmenuMain);
		  TreeView_SelectDropTarget(h_tree,NULL);
		  if (uMenuID){

			  CaseMenuPopup(uMenuID);
		  }

		return 0;
	}







HTREEITEM CTreeEditor::InsertCopy(HTREEITEM hSrc)
{
  HTREEITEM retNew =	InsertCopyTreeMain (hSrc);
	//
	if (TestFolder(hSrc))
	  	CopyChildren (retNew, hSrc);

	return retNew;
}


HTREEITEM CTreeEditor::MoveTree(HTREEITEM hDest, HTREEITEM hSrc)
{
  HTREEITEM retNew =	CopyTreeMain (hDest, hSrc);
		// If the item has subitems, copy them, too.
	//
	if (TestFolder(hSrc))
	  	CopyChildren (retNew, hSrc);


	TreeView_DeleteItem(h_tree,hSrc);
	return retNew;
}

HTREEITEM CTreeEditor::CopyTree(HTREEITEM hDest, HTREEITEM hSrc)
{
	    TVINSERTSTRUCT tvins; 	
	    TVITEM tvi_Sourse;
		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hSrc; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
  	HWND hwndTV;
    TVITEM tvi; 

    HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    HTREEITEM hPrevRootItem = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM;
 
    tvi.pszText = strTemp; 
    tvi.cchTextMax = strlen(strTemp); 
 
    tvi.iImage = tvi_Sourse.iImage; 
    tvi.iSelectedImage = tvi_Sourse.iSelectedImage; 

	int intLenStr = strlen((LPSTR)tvi_Sourse.lParam)+2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr,(LPSTR)tvi_Sourse.lParam);
    tvi.lParam = (LPARAM) ParStr; 


 
    tvins.item = tvi; 


//--------------------
//    insert into details 
//	
    if (TestFolder(hDest) == TRUE ) {
      tvins.hInsertAfter = (HTREEITEM) TVI_LAST; 
      tvins.hParent = hDest; 
	} else {
      tvins.hInsertAfter = hDest; 
      tvins.hParent = TreeView_GetParent(h_tree,hDest); 
    }
 
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
//        hti = TreeView_GetParent(hwndTV, hPrev); 
//        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
//        tvi.hItem = hti; 
//        tvi.iImage = mi_nClose; 
//        tvi.iSelectedImage = mi_nClose; 
//        TreeView_SetItem(hwndTV, &tvi); 
	//
    return hPrev;

}


HTREEITEM CTreeEditor::InsertCopyTreeMain(HTREEITEM hSrc)
{
	    TVINSERTSTRUCT tvins; 	
	    TVITEM tvi_Sourse;
        HTREEITEM hDest=hSrc;		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hSrc; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
  	HWND hwndTV;
    TVITEM tvi; 

    HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    HTREEITEM hPrevRootItem = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM;
 
    tvi.pszText = strTemp; 
    tvi.cchTextMax = strlen(strTemp); 
 
    tvi.iImage = tvi_Sourse.iImage; 
    tvi.iSelectedImage = tvi_Sourse.iSelectedImage; 
 

	int intLenStr = strlen((LPSTR)tvi_Sourse.lParam)+2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr,(LPSTR)tvi_Sourse.lParam);
    tvi.lParam = (LPARAM) ParStr; 


    tvins.item = tvi; 


      tvins.hInsertAfter = hDest; 
      tvins.hParent = TreeView_GetParent(h_tree,hDest); 
 
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
//        hti = TreeView_GetParent(hwndTV, hPrev); 
//        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
//        tvi.hItem = hti; 
//        tvi.iImage = mi_nClose; 
//        tvi.iSelectedImage = mi_nClose; 
//        TreeView_SetItem(hwndTV, &tvi); 
	//
    return hPrev;

}

int CTreeEditor::Lavel= 0;

HTREEITEM CTreeEditor::CopyTreeMain(HTREEITEM hDest, HTREEITEM hSrc)
{
	    TVINSERTSTRUCT tvins; 	
	    TVITEM tvi_Sourse;
		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hSrc; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
  	HWND hwndTV;
    TVITEM tvi; 

    HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    HTREEITEM hPrevRootItem = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM;
 
    tvi.pszText = strTemp; 
    tvi.cchTextMax = strlen(strTemp); 
 
    tvi.iImage = tvi_Sourse.iImage; 
    tvi.iSelectedImage = tvi_Sourse.iSelectedImage; 
 


	int intLenStr = strlen((LPSTR)tvi_Sourse.lParam)+2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr,(LPSTR)tvi_Sourse.lParam);
    tvi.lParam = (LPARAM) ParStr; 

 
    tvins.item = tvi; 


//--------------------
//    insert into details 
//	
    if (TestFolder(hDest) == TRUE ) {
		if (TestNoChildren(hDest)){
           tvins.hInsertAfter = (HTREEITEM) TVI_LAST; 
           tvins.hParent = hDest; 
		} else {
			if (TestNoExpaned(hDest)){
                tvins.hInsertAfter = hDest; 
                tvins.hParent = TreeView_GetParent(h_tree,hDest); 
			} else {
               tvins.hInsertAfter = (HTREEITEM) TVI_LAST; 
               tvins.hParent = hDest; 
			}
		}
	} else {
      tvins.hInsertAfter = hDest; 
      tvins.hParent = TreeView_GetParent(h_tree,hDest); 
    }
 
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
//        hti = TreeView_GetParent(hwndTV, hPrev); 
//        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
//        tvi.hItem = hti; 
//        tvi.iImage = mi_nClose; 
//        tvi.iSelectedImage = mi_nClose; 
//        TreeView_SetItem(hwndTV, &tvi); 
	//
    return hPrev;

}





VOID CTreeEditor::CopyChildren(HTREEITEM hDest, HTREEITEM hSrc)
{
	//
	// Get the first subitem.
	//
	HTREEITEM hItem;
	HTREEITEM hItemNew;
	HTREEITEM hItemBase;
	hItem = TreeView_GetChild(h_tree,hSrc);
	hItemBase = hItem;
	//
	// Do the same for other subitems of hSrc.
	//
	while (hItem != NULL) {
	   hItemNew = CopyTree(hDest, hItem);
		   if (TestFolder(hItem)){
	    	  CopyChildren (hItemNew, hItem);
		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	}
}

BOOL CTreeEditor::TestFolder(HTREEITEM hDest)
{
	    TVITEM tvi;
        tvi.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE ; 

        tvi.hItem = hDest; 
		BOOL ret = TreeView_GetItem(h_tree, &tvi); 
		if ((tvi.iImage == mi_nClose) || (tvi.iImage == mi_nOpen)){
			return TRUE;
		}
		return FALSE;
}

VOID CTreeEditor::ResizeWindowPanel()
{

	  DWORD wHightEdit = 0;
	  DWORD wHightUrl = 0;
	  DWORD wHightTree = InnerHeight;
	  DWORD wBorder = GetSystemMetrics(SM_CYBORDER);
 	  h_tree = GetDlgItem(IDC_TREE_CTRL);
	  hWndEditBoxName = GetDlgItem(IDC_EDIT_NAME);
	  hWndEditBoxURL  = GetDlgItem(IDC_EDIT_URL);
      
     RECT rect; 
     if (::GetWindowRect(hWndEditBoxName,&rect)){
		 wHightEdit = rect.bottom - rect.top ;
		 wHightTree = wHightTree - wHightEdit - wBorder;
	 }

	 
     if (::GetWindowRect(hWndEditBoxURL,&rect)){
		 wHightUrl = rect.bottom - rect.top ;
		 wHightTree = wHightTree - wHightUrl - wBorder;
	 }
  


	::MoveWindow(h_tree,0,0,InnerWidth,wHightTree,TRUE);
	::MoveWindow(hWndEditBoxName,0,wHightTree+wBorder,InnerWidth-wBorder,wHightEdit,TRUE);
	::MoveWindow(hWndEditBoxURL,0,wHightTree+wBorder+wHightEdit,InnerWidth-wBorder,wHightUrl,TRUE);

}

VOID CTreeEditor::SaveRestoreExpand(BOOL SaveFlad)
{

}



VOID CTreeEditor::TestFolderIcon(HTREEITEM hti)
{
    TVITEM tvi; 
    
        tvi.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE ; 

        tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE ; 
        tvi.hItem = hti; 

		TreeView_GetItem(h_tree, &tvi); 


//
		if ((tvi.iSelectedImage == mi_nOpen) ||  
			(  tvi.iImage == mi_nClose)) {

		if (tvi.state & TVIS_EXPANDED){
			if (TestNoChildren(hti)){
                tvi.hItem = hti; 
                tvi.iImage = mi_nClose; 
                tvi.iSelectedImage = mi_nClose; 
			} else {
                tvi.hItem = hti; 
                tvi.iImage = mi_nOpen; 
                tvi.iSelectedImage = mi_nOpen; 
			}
		} else {
                tvi.hItem = hti; 
                tvi.iImage = mi_nClose; 
                tvi.iSelectedImage = mi_nClose; 
		}
		TreeView_SetItem(h_tree, &tvi); 
	}

}

void CTreeEditor::InsertTopLavel(LPSTR lpszItem,int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = lstrlen(lpszItem); 
 
   
    // Save the heading level in the item's application-defined 
    // data area. 


	int intLenStr = 2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 


    tvi.iImage = mi_nClose; 
    tvi.iSelectedImage = mi_nClose; 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 
 
    // Set the parent item based on the specified level. 
  
        tvins.hParent = TVI_ROOT; 
   
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
		SteckItem[0]=hPrev;
		LenStectItem=1;
 
}



WNDPROC	gwpOrigEditProc;

///////////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL EditCtrlSubProc(HWND hWnd, WORD wMessage,WORD wParam,LONG lParam)
{
 int k;
 k = 1;
    switch (wMessage)
     {
 
          case WM_GETDLGCODE:
            return (DLGC_WANTALLKEYS |
                    CallWindowProc( (WNDPROC)gwpOrigEditProc, hWnd, wMessage,
                                   wParam, lParam));
 
          case WM_CHAR:
          //Process this message to avoid message beeps.
				if ((wParam == VK_RETURN) || (wParam == VK_TAB))
					return 0;
			break ;
 
       default:
            break;
 
     } /* end switch */


	return (CallWindowProc( (WNDPROC) gwpOrigEditProc, hWnd, wMessage,
                                   wParam, lParam));

}




void CTreeEditor::InsertNextFolder(LPSTR lpszItem, int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
  
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = lstrlen(lpszItem); 
 
   
    // Save the heading level in the item's application-defined 
    // data area.
	

	int intLenStr = 2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 



    tvi.iImage = mi_nClose; 
    tvi.iSelectedImage = mi_nClose; 
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
 
    // Set the parent item based on the specified level. 
	if (LenStectItem > 0 ) {
         tvins.hParent = SteckItem[LenStectItem-1]; 
	}else {
		tvins.hParent = TVI_ROOT;
	}
   
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 
		SteckItem[LenStectItem]=hPrev;
		LenStectItem++;

}

void CTreeEditor::InsertItem(LPSTR lpszItem, LPSTR lpszParameters)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
 
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = lstrlen(lpszItem); 
 
    // Assume the item is not a parent item, so give it a 
    // document image. 
    tvi.iImage = mi_Document; 
    tvi.iSelectedImage = mi_Document; 
 
    // Save the heading level in the item's application-defined 
    // data area. 

	int intLenStr = strlen(lpszParameters);
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr,lpszParameters);
    tvi.lParam = (LPARAM) ParStr; 
 
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 

	if (LenStectItem > 0 ) {
         tvins.hParent = SteckItem[LenStectItem-1]; 
	}else {
		tvins.hParent = TVI_ROOT;
	}
 
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
 


 
}

void CTreeEditor::TestChildFolderIcon(HTREEITEM hti)
{
	//
	// Get the first subitem.
	//
	HTREEITEM hItem;

	HTREEITEM hItemBase;
	hItem = TreeView_GetChild(h_tree,hti);
	hItemBase = hItem;
	//
	while (hItem != NULL) {
  	   TestFolderIcon(hItem);
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	}
}

void CTreeEditor::SaveTreeToFile(char *pFileName){

    SaveStart (pFileName);
	//
	//
	HTREEITEM hItem;
	HTREEITEM hItemBase;
	if (m_SaveCurrentFolder == FALSE ){
	    hItemBase =  TreeView_GetFirstVisible(h_tree); 

	hItem = hItemBase; //TreeView_GetChild(h_tree,hItemBase);
	//
	//
	while (hItem != NULL) {
		   if (TestFolder(hItem)){
	    	  SaveChidren(hItem, pFileName);
		   }else {
	          SaveItem(hItem, pFileName);
		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	}
	} else {
		hItemBase = m_EditTreeItem;
   	    SaveChidren(hItemBase, pFileName);
	}
    SaveEnd (pFileName);

}


void CTreeEditor::SaveStart (char *pFileName){
   FILE *stream;
   stream = fopen( pFileName, "w" );
   char strPut[255];
   switch( m_TypeSave ) 
   {
      case Type_SAVE:
           sprintf(strPut,"^M^%s\n",m_NameMenu);
	       fputs(strPut,stream);
         break;
      case Type_XML:
		  {
  		     Lavel = 1;
		     fputs("<?xml version=\"1.0\"?>\n",stream);
             fputs("<?xml:stylesheet type=\"text/xsl\" href=\"menus.xsl\"?>\n",stream);
             sprintf(strPut,"<TOPICLIST TYPE=\"%s\">\n",m_NameMenu);
             fputs(strPut,stream);
		  break;
		  }
      case Type_DHTML :
		fputs("<html>\n",stream);
		fputs("<head>\n",stream);

          sprintf(strPut,"<title>%s</title>\n",m_NameMenu);
	       fputs(strPut,stream);
        fputs("<meta name=\"GENERATOR\" content=\"Vladimir Novick. Tree Menu Generator\">\n",stream);
		fputs("<link rel=\"stylesheet\" href=\"NovTree.css\">\n",stream);
		fputs("<script  src=\"NovTree.js\"></script>\n",stream);
		fputs("<script>\n",stream);
		fputs("foldersTree = gFld(\" \", \"\")\n",stream);
		 m_LenSteckFolder= 0;
		 m_NewFolderIndex= 0;
         break;
   }


   fclose(stream);
}


void CTreeEditor::SaveEnd (char *pFileName){
	    FILE *stream;
	    stream = fopen( pFileName, "a" );

   switch( m_TypeSave ) 
   {
      case Type_SAVE:
         break;
	  case Type_XML:
		  fputs("</TOPICLIST>",stream);
		  break;
      case Type_DHTML :

		fputs("initializeDocument()\n",stream);
		fputs("</script>\n",stream);
		fputs("<title>Contents</title>\n",stream);
		fputs("<base target=\"MainFrame\">\n",stream);
		fputs("</head>\n",stream);
		fputs("<body BackGround=\"bkgFrame.jpg\" link=\"#000080\" vlink=\"#000080\" alink=\"#000080\">\n",stream);
		fputs("</body>\n",stream);
		fputs("</html>\n",stream);

         break;
   }
      fclose(stream);
}


void CTreeEditor::SaveItem (HTREEITEM retNew,char *pFileName){
    FILE *stream;
	char strName[255];
	char strPut[500];
	char strUrl[255];
	strUrl[0]= 0;
	int kl;
    GetItemAttr(retNew,strName,strUrl);


    stream = fopen( pFileName, "a" );


       switch( m_TypeSave ) 
	   {
        case Type_SAVE:
           sprintf(strPut,"^I^%s^166^%s\n",strName,strUrl);
	       fputs(strPut,stream);
           break;
		case Type_XML:
			{
				for (kl = 0; kl < Lavel; kl++){
					fputs("    ",stream);
				}
			 fputs("<TOPIC>",stream);
           sprintf(strPut,"<TITLE>%s</TITLE>",strName);
	       fputs(strPut,stream);
           sprintf(strPut,"<URL>%s</URL>",strUrl);
	       fputs(strPut,stream);
           fputs("</TOPIC>\n",stream);
			break;
			}
        case Type_DHTML :
			{
			  char strFldName[255];
			  char stFldInsFld[255];
			  strcpy(strFldName,"fld");
			  if (m_LenSteckFolder> 0 ){
				  sprintf(stFldInsFld,"fld%i",m_FolderSteck[m_LenSteckFolder]);
			  } else {
			      strcpy(stFldInsFld,"foldersTree");
			  }
			  sprintf(strFldName,"fld%i",m_FolderSteck[m_LenSteckFolder]);
  			  WebEncode(strName);
			  if (strUrl[3]==':') {
                 sprintf(strPut,"insDoc(%s, gLnk(2, \"%s\", \"%s\"))\n",stFldInsFld,strName,strUrl);
			  } else {
                 sprintf(strPut,"insDoc(%s, gLnk(3, \"%s\", \"%s\"))\n",stFldInsFld,strName,strUrl);
			  }
	          fputs(strPut,stream);

			}
           break;
	   }



    fclose(stream);
}



void CTreeEditor::SaveFolderItem(HTREEITEM retNew,char *pFileName){
    FILE *stream;
	char strPut[500];
	char strName[255];
	char strUrl[255];
	if (retNew == NULL ) {
	    //  end folder

       stream = fopen( pFileName, "a" );
       switch( m_TypeSave ) 
	   {
        case Type_SAVE:
	       fputs("^F^end\n",stream);
           break;
		case Type_XML:
			{
			Lavel--;
				for (int kl = 0; kl < Lavel; kl++){
					fputs("    ",stream);
				}
			fputs("</TOPICS>\n",stream);

			break;
			}
        case Type_DHTML :
			  if (m_LenSteckFolder> 0 ){
				  m_LenSteckFolder--;
			  }
           break;
	   }

       fclose(stream);


	} else {
		// begin folder
	   GetItemAttr(retNew,strName,strUrl);
       stream = fopen( pFileName, "a" );

       switch( m_TypeSave ) 
	   {
        case Type_SAVE:
	        sprintf(strPut,"^F^%s^100\n",strName);
	        fputs(strPut,stream);
           break;
		case Type_XML:
			{
				for (int kl = 0; kl < Lavel; kl++){
					fputs("    ",stream);
				}
	        sprintf(strPut,"<TOPICS TYPE=\"%s\">\n",strName);
	        fputs(strPut,stream);
			Lavel++;
			break;
			}
        case Type_DHTML :
			{
			  WebEncode(strName);
			  char strFldName[255];
			  char stFldInsFld[255];
			  strcpy(strFldName,"fld");
			  if (m_LenSteckFolder> 0 ){
				  sprintf(stFldInsFld,"fld%i",m_FolderSteck[m_LenSteckFolder]);
			  } else {
			      strcpy(stFldInsFld,"foldersTree");
			  }
			  m_LenSteckFolder++;
		      m_NewFolderIndex++;
			  m_FolderSteck[m_LenSteckFolder]=m_NewFolderIndex;
			  sprintf(strFldName,"fld%i",m_FolderSteck[m_LenSteckFolder]);
			  sprintf(strPut,"%s = insFld(%s, gFld(\"%s\", \"\"))\n",strFldName,stFldInsFld,strName);
	          fputs(strPut,stream);
			}
           break;
	   }





       fclose(stream);

	}
}


void CTreeEditor::SaveChidren (HTREEITEM retNew,char *pFileName){
	HTREEITEM hItem;

	HTREEITEM hItemBase =  retNew;
    SaveFolderItem(retNew,pFileName);
	hItem = TreeView_GetChild(h_tree,hItemBase);
	//
	//
	while (hItem != NULL) {
		   if (TestFolder(hItem)){
	    	  SaveChidren(hItem, pFileName);
		   }else {
	          SaveItem(hItem, pFileName);
		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	}
    SaveFolderItem(NULL,pFileName);
}

void CTreeEditor::LoadTreeFromFile(char *pFileName)
{
   LenStectItem = 0;
   FILE *stream;
   char seps[]   = "^\t\n";
   char *token;
   char s_File[255];
   strcpy(s_File,pFileName);	   
   long NFolder = 0;
   char line[600];
   char NameMenuSub1[240];
	if (boolClearAll == TRUE){
       strcpy(m_NameMenu,"Untitles");
	}

   if( (stream = fopen( s_File, "r" )) != NULL )
   {
	   fgets( line, 550, stream );
	   while (!feof( stream )){

 
   
         
   token = strtok( line, seps );
   if (token != NULL ){
//

//
//   folder 
//
   if (strcmp(token,"F")==0) {
      while( token != NULL )
	  {
          /* While there are tokens in "string" */
          //printf( " %s\n", token );
          /* Get next token: */
          token = strtok( NULL, seps );
		  if (strcmp(token,"end")==0){
			  InsertEndFolder();
  			  NFolder--;
			  break;
		  } else {
			  NFolder++;
			  InsertNextFolder(token,1);	
			  break;
		  }
		}
   }

//
   if (boolClearAll == TRUE){
   if (strcmp(token,"M")==0) {
          token = strtok( NULL, seps );
		  strcpy(m_NameMenu,token);
		  SetNameMenu();
   }
   }
   
   
//
//   Item 
//
   if (strcmp(token,"I")==0) {
          /* While there are tokens in "string" */
          //printf( " %s\n", token );
          /* Get next token: */
          token = strtok( NULL, seps );
		  strcpy(NameMenuSub1,token);
          token = strtok( NULL, seps );
		  char NameP[255];
		  NameP[0]='\0';
          token = strtok( NULL, seps );
		  strcpy(NameP,token);
          token = strtok( NULL, seps );

		  InsertItem(NameMenuSub1,NameP);	
   }
   }

		if( fgets( line, 550, stream ) == NULL){
		   break;
		}

        if (ferror( stream )){
			break;
		}

	   }
      fclose( stream );
   }
}



BOOL CTreeEditor::TestNoExpaned(HTREEITEM hti)
{
    TVITEM tvi; 
	BOOL Ret;
    
        tvi.mask = 
            TVIF_IMAGE |  
            TVIF_STATE | TVIF_SELECTEDIMAGE ; 

        tvi.hItem = hti; 

		TreeView_GetItem(h_tree, &tvi); 


//
		Ret = FALSE;

		if (tvi.iSelectedImage == mi_nClose){
			Ret  = TRUE;
		}
		return Ret;
}

DWORD CTreeEditor::GetItemID(HTREEITEM hti, char *Text,int &folder)
{
    TVITEM tvi; 
    
        tvi.mask = 
            TVIF_IMAGE |  
            TVIF_TEXT | TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE ; 

        tvi.hItem = hti; 
		tvi.pszText = Text; 
        tvi.cchTextMax = 128; 

		TreeView_GetItem(h_tree, &tvi); 


//
		folder = 0;

		if (tvi.iSelectedImage == mi_nOpen){
			folder = 1;
		}
		if (  tvi.iImage == mi_nClose) {
			folder = 2;
		}
		return 0;
}

//  ---- all -----
VOID CTreeEditor::ExpandAllW(DWORD Key) {
	//
	//
	HTREEITEM hItem;
	HTREEITEM hItemBase;
	    hItemBase =  TreeView_GetRoot(h_tree); 

	hItem = hItemBase; //TreeView_GetChild(h_tree,hItemBase);
	//
	//
	while (hItem != NULL) {
		   if (TestFolder(hItem)){
              TreeView_Expand(h_tree,hItem,Key);
              ExpandChildren(hItem,Key);
              TestFolderIcon(hItem);
		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	}
}
// ---------------

VOID CTreeEditor::ExpandAll(HTREEITEM hti,DWORD Key)
{
  TreeView_Expand(h_tree,hti,Key);
  TestFolderIcon(hti);
  ExpandChildren(hti,Key);
}

VOID CTreeEditor::ExpandChildren(HTREEITEM hti,DWORD Key)
{
 	//
	// Get the first subitem.
	//
	HTREEITEM hItem;
	HTREEITEM hItemBase;
	hItem = TreeView_GetChild(h_tree,hti);
	hItemBase = hItem;
	//
	// Do the same for other subitems of hSrc.
	//
	while (hItem != NULL) {
		   if (TestFolder(hItem)){
              TreeView_Expand(h_tree,hItem,Key);
              TestFolderIcon(hItem);
	    	  ExpandChildren(hItem,Key);
		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	} 
}

BOOL CTreeEditor::TestNoExpandAll(HTREEITEM hti)
{
 return TestNoExpandChildren(hti);
}

BOOL CTreeEditor::TestNoChildren(HTREEITEM hti)
{
	HTREEITEM hItem;
	hItem = TreeView_GetChild(h_tree,hti);
	if (hItem){ return FALSE;} else
	{return TRUE;}
}


BOOL CTreeEditor::TestNoExpandChildren(HTREEITEM hti)
{
 	//
	// Get the first subitem.
	//
	BOOL Ret;
	HTREEITEM hItem;
	HTREEITEM hItemBase;
	hItem = TreeView_GetChild(h_tree,hti);
	hItemBase = hItem;
	//
	// Do the same for other subitems of hSrc.
	//
	Ret = FALSE;
	while (hItem != NULL) {
		   if (TestFolder(hItem)){
			  Ret = TestNoExpaned(hItem);
				  if (Ret){
					  break;
				  }
	    	  Ret = TestNoExpandChildren(hItem);
				  if (Ret){
					  break;
				  }

		   }
	   hItem = TreeView_GetNextItem(h_tree,hItem,TVGN_NEXT );
	} 
	return Ret;
}


LRESULT CTreeEditor::OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	   	bHandled = FALSE ;
  	    char MenuFileName[MAX_PATH];
		char *szFilter = "Internet Tree Menu (*.mnu;*.mnt)\0*.mnu;*.mnt\0All Files (*.*)\0*.*\0";
		TCHAR filterF[20];
		strcpy(filterF,_T("*.mnu;*.mnt;*.*"));
        MenuFileName[0] = '\0';
		switch( wParam ) 
	{
	    case ID_FILE_NEW:
			{
		       m_EditTreeItem = NULL;
   	           TreeView_DeleteAllItems(GetDlgItem(IDC_TREE_CTRL));
 		       strcpy(CurrentFileName,"Untitles.mnu");
		       strcpy(m_NameMenu,"Untitles");
			   SetNameMenu();
			}
			break;
		case ID_INSERT_DELETEALL:
		       m_EditTreeItem = NULL;
		       if (::MessageBox (this->m_hWnd ,"Delete all items and all folders ?", TEXT("Question"), MB_OKCANCEL| MB_ICONQUESTION)==IDOK){
   	               TreeView_DeleteAllItems(GetDlgItem(IDC_TREE_CTRL));
			   }


			break;
		case ID_EDIT_INSERTFILE:
			boolClearAll = FALSE;
	    case ID_FILE_OPENFILE:
			{
			   OPENFILENAME ofn;
		   ofn.lStructSize = sizeof(ofn);
		   ofn.hwndOwner = NULL; 
		   ofn.lpstrFile = MenuFileName; 
		   ofn.nMaxFile = MAX_PATH;
           ofn.lpstrFilter  =    szFilter; 
    
		   ofn.nFilterIndex = 1;
		   ofn.lpstrTitle = TEXT("Please Select a File");
		   ofn.lpstrInitialDir = NULL;
		   ofn.lpstrCustomFilter = NULL;
		   ofn.nMaxCustFilter = 0;
		   ofn.lpstrFileTitle = NULL;
		   ofn.nMaxFileTitle = 0;
		   ofn.nFileOffset = 0;
		   ofn.nFileExtension = 0;
		   ofn.lpstrDefExt = NULL;
		   ofn.lCustData = 0;
		   ofn.lpfnHook = 0;
		   ofn.lpTemplateName = 0;
		   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;



   if (GetOpenFileName(&ofn))
   {
		 m_EditTreeItem = NULL;
		 if ( boolClearAll == TRUE){
	        TreeView_DeleteAllItems(GetDlgItem(IDC_TREE_CTRL));
		 }
    	 LoadTreeFromFile(MenuFileName);
		 if (boolClearAll == TRUE){
		   strcpy(CurrentFileName,MenuFileName);
		   SetNameMenu();
		 }

   }
			boolClearAll = TRUE;
   


			}
            break;
		case ID_HELP_ABOUT:
			{
				CCAbout nAbout;
				strcpy(nAbout.m_NameMenu,m_NameMenu);
				nAbout.DoModal(this->m_hWnd);
				strcpy(m_NameMenu,nAbout.m_NameMenu);
				SetNameMenu();
			}
			break;


		case ID_HELP_HELP:
			{
           ShellExecute(NULL, "open", "http://www.deltacom.co.il/nv/index.html", NULL, NULL, SW_SHOWNORMAL);

			}
			break;


		case ID_EDIT_SAVEFOLDERTOFILE:
			m_SaveCurrentFolder = TRUE;

	    case ID_FILE_SAVE:
			{
				if (m_SaveCurrentFolder == FALSE ){
				if (strcmp(CurrentFileName,"Untitles.mnu") != 0 ){
				   TCHAR szText[MAX_PATH] = TEXT("\0");
				   strcpy(szText,"Write :\n");
				   strcat(szText,CurrentFileName);
				   if (::MessageBox (this->m_hWnd ,szText, TEXT("Question"), MB_OKCANCEL| MB_ICONQUESTION)==IDOK){
					  m_TypeSave =  Type_SAVE;
 		              SaveTreeToFile(CurrentFileName);
				   }
                 break;
				}
				}
			}

	    case ID_FILE_SAVEAS:
			{
				TCHAR szFile[MAX_PATH] = TEXT("\0");
				 OPENFILENAME ofn;
  
				 memset (&ofn, 0, sizeof (ofn));
				 ofn.lStructSize  = sizeof (OPENFILENAME);
				 ofn.hwndOwner = this->m_hWnd ;
				 ofn.lpstrFile = szFile;
				 ofn.nMaxFile = MAX_PATH;  

		          ofn.lpstrFilter = szFilter;  
		          ofn.lpstrTitle = TEXT("Save File As");
		          ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
                      OFN_PATHMUSTEXIST; 
		          ofn.lpstrDefExt = TEXT("mnu");

		          if (GetSaveFileName (&ofn))  
		          {
					  m_TypeSave =  Type_SAVE;
					  SaveTreeToFile(szFile);
					  if (m_SaveCurrentFolder == FALSE){
   		                 strcpy(CurrentFileName,szFile);
                         SetNameMenu();
					  }
          
		          }
			m_SaveCurrentFolder = FALSE;


			}
            break;
        case ID_EXPORT_XMLMENULIST:
			{
				TCHAR szFile[MAX_PATH] = TEXT("\0");
				 OPENFILENAME ofn;
		         char *szFilterDHTML = "XML file (*.xml)\0*.xml\0All Files (*.*)\0*.*\0";
  
				 memset (&ofn, 0, sizeof (ofn));
				 ofn.lStructSize  = sizeof (OPENFILENAME);
				 ofn.hwndOwner = this->m_hWnd ;
				 ofn.lpstrFile = szFile;
				 ofn.nMaxFile = MAX_PATH;  

		          ofn.lpstrFilter = szFilterDHTML;  
		          ofn.lpstrTitle = TEXT("Export in the XML file");
		          ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
                      OFN_PATHMUSTEXIST; 
		          ofn.lpstrDefExt = TEXT("xml");

		          if (GetSaveFileName (&ofn))  
		          {
					  m_TypeSave =  Type_XML;
					  SaveTreeToFile(szFile);
       
		          }

				break;
			}
		case ID_EXPORT_HTMLTREEVIEWPAGE:
			{
				TCHAR szFile[MAX_PATH] = TEXT("\0");
				 OPENFILENAME ofn;
		         char *szFilterDHTML = "html page (*.htm;*.html)\0*.htm;*.html\0All Files (*.*)\0*.*\0";
  
				 memset (&ofn, 0, sizeof (ofn));
				 ofn.lStructSize  = sizeof (OPENFILENAME);
				 ofn.hwndOwner = this->m_hWnd ;
				 ofn.lpstrFile = szFile;
				 ofn.nMaxFile = MAX_PATH;  

		          ofn.lpstrFilter = szFilterDHTML;  
		          ofn.lpstrTitle = TEXT("Export in the DHTML page");
		          ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
                      OFN_PATHMUSTEXIST; 
		          ofn.lpstrDefExt = TEXT("htm");

		          if (GetSaveFileName (&ofn))  
		          {
					  m_TypeSave =  Type_DHTML;
					  SaveTreeToFile(szFile);
       
		          }


			 break;
			}
	    case ID_FILE_EXIT:
			EndDialog(0);
            break;
        case ID_POPMENUTREE_COLLAPSED_N:
			ExpandAllW(TVE_COLLAPSE);
			break;
		case ID_POPMENUTREE_EXPANDALL_N:
			ExpandAllW(TVE_EXPAND);
			break;
	}


       CaseMenuPopup( wParam );

		return 0;
	}


HTREEITEM CTreeEditor::InsertNewFolder(LPSTR lpszItem, int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = NULL; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	HTREEITEM hDest= NULL;
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
  


       tvi.mask = TVIF_TEXT | TVIF_IMAGE 
         | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
         // Set the text of the item. 
         tvi.pszText = lpszItem; 
         tvi.cchTextMax = lstrlen(lpszItem); 
 
   
		    // Save the heading level in the item's application-defined 
		    // data area. 


	int intLenStr = 5;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 

		    tvi.iImage = mi_nClose; 
		    tvi.iSelectedImage = mi_nClose; 
		    tvins.item = tvi; 
		    tvins.hInsertAfter = TVI_LAST; 



     hDest=TreeView_GetSelection(hwndTV);
     if (hDest) {
		    // Set the parent item based on the specified level. 
		    if (TestFolder(hDest) == TRUE ) {
		      tvins.hInsertAfter = (HTREEITEM) TVI_LAST; 
		      tvins.hParent = hDest; 
			} else {
		      tvins.hInsertAfter = hDest; 
		      tvins.hParent = TreeView_GetParent(h_tree,hDest); 
			}
	 } else {
 	   tvins.hParent = TVI_ROOT;
       tvins.hInsertAfter = TVI_LAST; 

	 }

   
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
	return hPrev;
}



HTREEITEM CTreeEditor::InsertNewTopFolder(LPSTR lpszItem, int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = NULL; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	HTREEITEM hDest= NULL;
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
  


       tvi.mask = TVIF_TEXT | TVIF_IMAGE 
         | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
         // Set the text of the item. 
         tvi.pszText = lpszItem; 
         tvi.cchTextMax = lstrlen(lpszItem); 
 
   
		    // Save the heading level in the item's application-defined 
		    // data area.
		 

	int intLenStr = 2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 


		    tvi.iImage = mi_nClose; 
		    tvi.iSelectedImage = mi_nClose; 
		    tvins.item = tvi; 
		    tvins.hInsertAfter = TVI_LAST; 


 	   tvins.hParent = TVI_ROOT;
       tvins.hInsertAfter = TVI_LAST; 


   
    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
	return hPrev;
}





HTREEITEM CTreeEditor::InsertNewItem(LPSTR lpszItem, int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = NULL; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	HTREEITEM hDest= NULL;
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = lstrlen(lpszItem); 
 
   
    // Save the heading level in the item's application-defined 
    // data area. 



	int intLenStr = 2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 




    tvi.iImage = mi_Document; 
    tvi.iSelectedImage = mi_Document; 
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
  
    hDest=TreeView_GetSelection(hwndTV);
    if (hDest) {
 
    // Set the parent item based on the specified level. 
    if (TestFolder(hDest) == TRUE ) {
      tvins.hInsertAfter = (HTREEITEM) TVI_LAST; 
      tvins.hParent = hDest; 
	} else {
      tvins.hInsertAfter = hDest; 
      tvins.hParent = TreeView_GetParent(h_tree,hDest); 
    }
   
    } else {
	   tvins.hParent = TVI_ROOT;
       tvins.hInsertAfter = TVI_LAST; 
   }



    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
	return hPrev;
}





HTREEITEM CTreeEditor::InsertNewTopItem(LPSTR lpszItem, int IndexFolder)
{
	HWND hwndTV;
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    HTREEITEM hPrev = NULL; 
    HTREEITEM hPrevRootItem = NULL; 
    HTREEITEM hPrevLev2Item = NULL; 
	HTREEITEM hDest= NULL;
	hwndTV = GetDlgItem(IDC_TREE_CTRL);
    tvi.mask = TVIF_TEXT | TVIF_IMAGE 
        | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
 
    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = lstrlen(lpszItem); 
 
   
    // Save the heading level in the item's application-defined 
    // data area. 


	int intLenStr = 2;
    LPSTR ParStr = (LPSTR)malloc(intLenStr+2);
	strcpy(ParStr," ");
    tvi.lParam = (LPARAM) ParStr; 



    tvi.iImage = mi_Document; 
    tvi.iSelectedImage = mi_Document; 
    tvins.item = tvi; 
    tvins.hInsertAfter = TVI_LAST; 
  
	   tvins.hParent = TVI_ROOT;
       tvins.hInsertAfter = TVI_LAST; 



    // Add the item to the tree view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, 
         (LPARAM) (LPTVINSERTSTRUCT) &tvins); 
	return hPrev;
}




HTREEITEM CTreeEditor::DeleteItem(HTREEITEM hti)
{
	HTREEITEM htiCurrent = NULL;
    htiCurrent = TreeView_GetNextItem(h_tree,hti,TVGN_NEXT );
	if (htiCurrent == NULL){
      htiCurrent = TreeView_GetNextItem(h_tree,hti,TVGN_PREVIOUS );
	  if (htiCurrent == NULL){
	     htiCurrent = TreeView_GetParent(h_tree,hti);
	  }
	}
	TreeView_DeleteItem(h_tree,hti);
	return htiCurrent;
}


	LRESULT CTreeEditor::OnDeleteitemTree_ctrl(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
	m_EditTreeItem = NULL;
	 LPNMTREEVIEW lpnmtv = (NMTREEVIEW *) pnmh; 
 

//	*pResult = 0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*) pnmh;

	//
	// Do nothing if the user is attempting to drag a top-level item.
	//
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;
//         TreeView_SelectDropTarget(h_tree,NULL);

//-----

	    TVITEM tvi_Sourse;
		



		char strTemp[255];
		strTemp[0]=0;

        tvi_Sourse.mask = 
            TVIF_IMAGE |  
            TVIF_PARAM |
            TVIF_STATE | TVIF_SELECTEDIMAGE |
            TVIF_TEXT ;


        tvi_Sourse.hItem = hItem; 
        tvi_Sourse.pszText = strTemp; 
        tvi_Sourse.cchTextMax = 250; 
		TreeView_GetItem(h_tree, &tvi_Sourse); 
//-----
        LPSTR ParStr = (LPSTR)tvi_Sourse.lParam;
        free(ParStr);


		::SetWindowText(hWndEditBoxName,"");
		::SetWindowText(hWndEditBoxURL,"");
		return 0;
	}



void CTreeEditor::CaseMenuPopup(DWORD uMenuID)
{
	HTREEITEM	hti;
	HTREEITEM hCurrentFolder;
	hti = m_EditTreeItem;
			switch( uMenuID )
			{
				// item
			case ID_POPMENUTREE_EXPANDALL:
				ExpandAll(hti,TVE_EXPAND);
				MSelect(hti);
				break;

			case ID_POPMENUTREE_COLLAPSED:
				ExpandAll(hti,TVE_COLLAPSE);
			    TreeView_SelectItem(h_tree,hti);
				break;
			case ID_POPMENUTREE_DELETE:
				    hCurrentFolder = DeleteItem(hti);
	                TreeView_SelectItem(h_tree,hCurrentFolder);
				    TestFolderIcon(hCurrentFolder);
				    MSelect(hCurrentFolder);
				break;


			case ID_POPMENUTREE_INSERTITEM:
				hCurrentFolder = TreeView_GetSelection(h_tree);
				hti = InsertNewItem("New item",0);
	            TreeView_SelectItem(h_tree,hti);
				TestFolderIcon(hCurrentFolder);
				TestFolderIcon(hti);
				MSelect(hti);
				break;

			case ID_POPMENUTREE_INSERTCOPY:
				hCurrentFolder = TreeView_GetSelection(h_tree);
				hti = InsertCopy(hCurrentFolder);
	            TreeView_SelectItem(h_tree,hti);
				TestFolderIcon(hti);
				MSelect(hti);
				break;

			case ID_POPMENUTREE_INSERTTOPFOLDER:
				hCurrentFolder = TreeView_GetSelection(h_tree);
				hti = InsertNewTopFolder("New Folder",0);
				TestFolderIcon(hCurrentFolder);
				TestFolderIcon(hti);
				MSelect(hti);
				break;

			case ID_POPMENUTREE_INSERTTOPITEM:
				hCurrentFolder = TreeView_GetSelection(h_tree);
				hti = InsertNewTopItem("New item",0);
				TestFolderIcon(hCurrentFolder);
				TestFolderIcon(hti);
				MSelect(hti);
				break;

			case ID_POPMENUTREE_INSERTFOLDER:
				hCurrentFolder = TreeView_GetSelection(h_tree);
				hti = InsertNewFolder("New Folder",0);
				TestFolderIcon(hCurrentFolder);
				TestFolderIcon(hti);
				MSelect(hti);
				break;
				
			}

}

void CTreeEditor::MSelect(HTREEITEM hti)
{
      TreeView_SelectItem(h_tree,hti);

}

void CTreeEditor::WebEncode(char *strSrc)
{
 char strBuf[500];
 char strOut[500];
 char charWork;
 int lenOut = 0;
 strcpy(strBuf,strSrc);
 int lenStr = strlen(strBuf);
 for (int i=0;i < lenStr;i++){
	 charWork = strBuf[i];
  switch( charWork ) 
	{
     case '"':
	   strOut[lenOut]='\\';       
	   lenOut++;
	   strOut[lenOut]=charWork;       
	   lenOut++;
        break;
     default :
	   strOut[lenOut]=charWork;       
	   lenOut++;
	}
 
 }
 	   strOut[lenOut]='\0';       
   strcpy(strSrc,strOut);
}

VOID CTreeEditor::SetNameMenu()
{
		  char TextSend[255];
		  strcpy(TextSend,"Internet Menu Editor - [");
          strcat(TextSend,m_NameMenu);
          strcat(TextSend,"]");
		  SetWindowText(TextSend);
}
