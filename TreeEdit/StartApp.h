// StartApp.h: interface for the StartApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARTAPP_H__ADAD67B2_4752_11D4_9F4B_005004816FFF__INCLUDED_)
#define AFX_STARTAPP_H__ADAD67B2_4752_11D4_9F4B_005004816FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StartApp  
{
public:
    void *ObjectWindowTree;
	HWND Run();
	StartApp();
	virtual ~StartApp();

};

#endif // !defined(AFX_STARTAPP_H__ADAD67B2_4752_11D4_9F4B_005004816FFF__INCLUDED_)
