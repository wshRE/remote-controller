#pragma once


// CEditCmd

class CEditCmd : public CEdit
{
	DECLARE_DYNAMIC(CEditCmd)

public:
	CEditCmd();
	virtual ~CEditCmd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//SOCKET m_sockClient;
	BOOL m_bCmd = FALSE;

	void Thread();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


