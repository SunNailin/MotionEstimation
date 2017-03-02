#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被覆盖。

/////////////////////////////////////////////////////////////////////////////
// COcx1 包装类

class COcx1 : public CWnd
{
protected:
	DECLARE_DYNCREATE(COcx1)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x6BF52A52, 0x394A, 0x11D3, { 0xB1, 0x53, 0x0, 0xC0, 0x4F, 0x79, 0xFA, 0xA6 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 特性
public:
enum
{
    mpReadyStateUninitialized = 0,
    mpReadyStateLoading = 1,
    mpReadyStateInteractive = 3,
    mpReadyStateComplete = 4
}MPReadyStateConstants;
enum
{
    mpStopped = 0,
    mpPaused = 1,
    mpPlaying = 2,
    mpWaiting = 3,
    mpScanForward = 4,
    mpScanReverse = 5,
    mpClosed = 6
}MPPlayStateConstants;
enum
{
    mpDefaultSize = 0,
    mpHalfSize = 1,
    mpDoubleSize = 2,
    mpFullScreen = 3,
    mpFitToSize = 4,
    mpOneSixteenthScreen = 5,
    mpOneFourthScreen = 6,
    mpOneHalfScreen = 7
}MPDisplaySizeConstants;
enum
{
    mpTime = 0,
    mpFrames = 1
}MPDisplayModeConstants;
enum
{
    mpShowURL = 0,
    mpClipURL = 1,
    mpBannerURL = 2
}MPMoreInfoType;
enum
{
    mpShowFilename = 0,
    mpShowTitle = 1,
    mpShowAuthor = 2,
    mpShowCopyright = 3,
    mpShowRating = 4,
    mpShowDescription = 5,
    mpShowLogoIcon = 6,
    mpClipFilename = 7,
    mpClipTitle = 8,
    mpClipAuthor = 9,
    mpClipCopyright = 10,
    mpClipRating = 11,
    mpClipDescription = 12,
    mpClipLogoIcon = 13,
    mpBannerImage = 14,
    mpBannerMoreInfo = 15,
    mpWatermark = 16
}MPMediaInfoType;
enum
{
    dvdMenu_Title = 2,
    dvdMenu_Root = 3,
    dvdMenu_Subpicture = 4,
    dvdMenu_Audio = 5,
    dvdMenu_Angle = 6,
    dvdMenu_Chapter = 7
}DVDMenuIDConstants;
enum
{
    mpShowDialogHelp = 0,
    mpShowDialogStatistics = 1,
    mpShowDialogOptions = 2,
    mpShowDialogContextMenu = 3
}MPShowDialogConstants;


// 操作
public:



};
