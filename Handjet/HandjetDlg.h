
// HandjetDlg.h : 头文件
//

#pragma once

#include "ImageProcessor.h"

// CHandjetDlg 对话框
class CHandjetDlg : public CDialogEx
{
// 构造
public:
	CHandjetDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HANDJET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	ImageProcessor* m_ImageProcessor;
	bool checked;
	bool showVideo;
	bool blueMethod;


	HCURSOR m_cursor_fw;
	HCURSOR m_cursor_backup_fw;
	HCURSOR m_cursor_mw;
	HCURSOR m_cursor_backup_mw;
	HCURSOR m_cursor_hw;
	HCURSOR m_cursor_backup_hw;
	HCURSOR m_cursor_hb;
	HCURSOR m_cursor_backup_hb;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheck2();
};
