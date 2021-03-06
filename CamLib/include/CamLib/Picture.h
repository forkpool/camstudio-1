//-----------------------------------------------------------------------------
// Picture (Prototypes) Version 1.00
//
// Routines 4 Showing Picture Files... (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//=============================================================================

#pragma once

/////////////////////////////////////////////////////////////////////////////
// DEPRECEATED: This class should be replaced by the CXImage library classes
/////////////////////////////////////////////////////////////////////////////
class CPicture
{
public:
    CPicture();
    virtual ~CPicture();

    BOOL Load(const CString &sFilePathName);
    BOOL LoadFromFile(FILE *fptr);
    BOOL SaveToFile(FILE *fptr);
    BOOL CopyToPicture(CPicture *dstPic, const CString &exchangeFile);

    IPicture *IPicturePtr();
    LONG Height() const;
    LONG Width() const;

protected:
    void FreePictureData();
    BOOL Load(UINT ResourceName, LPCTSTR ResourceType);
    BOOL LoadPictureData(BYTE *pBuffer, int nSize);
    BOOL SaveAsBitmap(const CString &sFilePathName);
    BOOL Show(CDC *pDC, CPoint LeftTop, CPoint WidthHeight, int MagnifyX, int MagnifyY);
    BOOL Show(CDC *pDC, CRect DrawRect);
    BOOL ShowBitmapResource(CDC *pDC, int BMPResource, CPoint LeftTop);
    BOOL UpdateSizeOnDC(CDC *pDC);

protected:
    IPicture *m_IPicture{nullptr}; // Same As LPPICTURE (typedef IPicture __RPC_FAR *LPPICTURE)

    LONG m_Height{0}; // Height (In Pixels Ignore What Current Device Context Uses)
    LONG m_Width{0};  // Width (In Pixels Ignore What Current Device Context Uses)
    LONG m_Weight{0}; // Size Of The Image Object In Bytes (File OR Resource)
};
