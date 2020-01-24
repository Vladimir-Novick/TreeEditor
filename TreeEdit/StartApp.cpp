// StartApp.cpp: implementation of the StartApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "commctrl.h"
#include "TreeEditor1.h"
#include "windowsx.h"
#include "TreeEditor1.h"
#include "StartApp.h"
//#include "commctrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StartApp::StartApp()
{

}

StartApp::~StartApp()
{

}

HWND StartApp::Run()
{
      CTreeEditor *pD = new CTreeEditor;
       pD->DoModal(::GetDesktopWindow());
	   ObjectWindowTree = ( void *)pD;
return pD->m_hWnd ;
}

