// TODO: this whole modeule needs cleanup
#include "stdafx.h"
#include "Recorder.h"
#include "ScreenAnnotationsDlg.h"
#include "ListManager.h"
#include "NewShapeDlg.h"
#include "LayoutList.h"
#include "KeyshortcutsDlg.h"
#include <CamLib/CStudioLib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_iNewShapeWidth;
extern int g_iNewShapeHeight;
extern CString g_strNewShapeText;
extern CString g_strImageFilename;
extern int g_iImageType;
extern CString g_shapeStr;

extern CString g_shapeName;

extern CString g_strLayoutName;
extern int g_keySCOpened;

extern INT_PTR g_iCurrentLayout;

extern int SetAdjustHotKeys();

namespace
{ // anonymous

const int modeShape = 0;
const int modeLayout = 1;

int objectIsCopied = 0;
int layoutIsCopied = 0;
int tabMode = modeShape;
int shapeLibraryMode = 0;  // default shape library mode is 0, if >0 ==> opened library
int layoutLibraryMode = 0; // default layout library mode is 0, if >0 ==> opened library

int newObjectOn = 0;

LV_ITEM itemCopied;
CTransparentWnd *wndCopied = nullptr;
CLayoutList *layoutCopied = nullptr;

int draggingOn = 0;
int iDragIndex = -1;

CImageList saImageList;
int saImageListLoaded = 0;

} // namespace

void AdjustShapeName(CString &shapeName);

/////////////////////////////////////////////////////////////////////////////
// CScreenAnnotationsDlg dialog

CScreenAnnotationsDlg::CScreenAnnotationsDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(CScreenAnnotationsDlg::IDD, pParent)
    , m_bEditingLabelOn(false)
    , m_hCursorDrag(0)
    , m_hCursorArrow(0)
{
    //{{AFX_DATA_INIT(CScreenAnnotationsDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CScreenAnnotationsDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CScreenAnnotationsDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_LIST1, m_ctrlList);
    DDX_Control(pDX, IDC_TAB1, m_ctrlTab);
}

BEGIN_MESSAGE_MAP(CScreenAnnotationsDlg, CDialog)
//{{AFX_MSG_MAP(CScreenAnnotationsDlg)
ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST1, OnBegindragList1)
ON_WM_CONTEXTMENU()
ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
ON_COMMAND(ID_EDITOBJ_NEWOBJECT, OnEditobjNewobject)
ON_COMMAND(ID_EDITOBJ_EDITIMAGE, OnEditobjEditimage)
ON_COMMAND(ID_EDITOBJ_EDITTEXT, OnEditobjEdittext)
ON_COMMAND(ID_EDITOBJ_EDITTRANSPARENCY, OnEditobjEdittransparency)
ON_COMMAND(ID_EDITOBJ_COPY, OnEditobjCopy)
ON_COMMAND(ID_EDITOBJ_PASTE, OnEditobjPaste)
ON_COMMAND(ID_EDITOBJ_REMOVE, OnEditobjRemove)
ON_COMMAND(ID_EDITOBJ_TESTEDIT, OnEditobjTestedit)
ON_WM_KEYDOWN()
ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, OnEndlabeleditList1)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnKeydownList1)
ON_WM_DESTROY()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
ON_COMMAND(ID_EDITLAYOUT_SAVELAYOUT, OnEditlayoutSavelayout)
ON_COMMAND(ID_EDITLAYOUT_OPENLAYOUT, OnEditlayoutOpenlayout)
ON_COMMAND(ID_EDITLAYOUT_CLOSEALLOBJECTS, OnEditlayoutCloseallobjects)
ON_COMMAND(ID_EDITLAYOUT_LAYOUTSHORTCUTS, OnEditlayoutLayoutshortcuts)
ON_COMMAND(ID_EDITOBJ_LIBRARY_CLOSELIBRARY, OnEditobjLibraryCloselibrary)
ON_COMMAND(ID_EDITOBJ_LIBRARY_OPENLIBRARY, OnEditobjLibraryOpenlibrary)
ON_COMMAND(ID_EDITOBJ_LIBRARY_NEWLIBRARY, OnEditobjLibraryNewlibrary)
ON_COMMAND(ID_EDITLAYOUT_LIBRARY_NEWLIBRARY, OnEditlayoutLibraryNewlibrary)
ON_COMMAND(ID_EDITLAYOUT_LIBRARY_OPENLIBRARY, OnEditlayoutLibraryOpenlibrary)
ON_COMMAND(ID_EDITLAYOUT_LIBRARY_CLOSELIBRARY, OnEditlayoutLibraryCloselibrary)
ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
ON_COMMAND(ID_OPTIONS_CLOSESCREENANNOTATIONS, OnOptionsClosescreenannotations)
ON_BN_CLICKED(IDNEXT, OnNext)
ON_COMMAND(ID_HELP_SHAPES, OnHelpShapes)
ON_COMMAND(ID_HELP_SHAPETOPICS_INSTANTIATINGASHAPE, OnHelpShapetopicsInstantiatingashape)
ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGASHAPE, OnHelpShapetopicsEditingashape)
ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGTEXT, OnHelpShapetopicsEditingtext)
ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGIMAGE, OnHelpShapetopicsEditingimage)
ON_COMMAND(ID_HELP_SHAPETOPICS_CREATINGANEWSHAPE, OnHelpShapetopicsCreatinganewshape)
ON_COMMAND(ID_HELP_SHAPETOPICS_EDITINGTRANSPARENCY, OnHelpShapetopicsEditingtransparency)
ON_COMMAND(ID_HELP_SHAPETOPICS_MANAGINGSHAPES, OnHelpShapetopicsManagingshapes)
ON_COMMAND(ID_HELP_SHAPETOPICS_RESIZINGSHAPES, OnHelpShapetopicsResizingshapes)
ON_COMMAND(ID_HELP_LAYOUTS, OnHelpLayouts)
ON_COMMAND(IDR_ANN_SAVELAYOUT, OnAnnSavelayout)
ON_WM_INITMENUPOPUP()
ON_COMMAND(ID_LIBRARY_OPENSHAPELIBRARY, OnLibraryOpenshapelibrary)
ON_COMMAND(ID_LIBRARY_SAVESHAPELIBRARY, OnLibrarySaveshapelibrary)
ON_COMMAND(ID_LIBRARY_NEWSHAPELIBRARY, OnLibraryNewshapelibrary)
ON_COMMAND(ID_LIBRARY_NEWLAYOUTLIBRARY, OnLibraryNewlayoutlibrary)
ON_COMMAND(ID_LIBRARY_OPENLAYOUTLIBRARY, OnLibraryOpenlayoutlibrary)
ON_COMMAND(ID_LIBRARY_SAVELAYOUTLIBRARY, OnLibrarySavelayoutlibrary)
ON_COMMAND(ID_OBJECTS_COPY, OnObjectsCopy)
ON_COMMAND(ID_OBJECTS_PASTE, OnObjectsPaste)
ON_COMMAND(ID_OBJECTS_REMOVE, OnObjectsRemove)
ON_WM_CREATE()
ON_COMMAND(ID_OBJECTS_EDITTEXT, OnObjectsEdittext)
ON_COMMAND(ID_OBJECTS_EDITIMAGE, OnObjectsEditimage)
ON_COMMAND(ID_OBJECTS_EDITNAME, OnObjectsEditname)
ON_COMMAND(ID_OBJECTS_EDITTRANSAPRENCY, OnObjectsEdittransaprency)
ON_COMMAND(ID_OBJECTS_CLOSEALLOBJECTS, OnObjectsCloseallobjects)
ON_COMMAND(ID_LIBRARY_INSERTSHAPELIBARAY, OnLibraryInsertshapelibaray)
ON_COMMAND(ID_OBJECTS_MOVEITEMUP, OnObjectsMoveitemup)
ON_COMMAND(ID_OBJECTS_MOVEITEMDOWN, OnObjectsMoveitemdown)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenAnnotationsDlg message handlers

void CScreenAnnotationsDlg::OnOK()
{
    ShowWindow(SW_HIDE);
    // CDialog::OnOK();
}

BOOL CScreenAnnotationsDlg::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();

    m_hCursorDrag = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORDRAG));
    m_hCursorArrow = ::LoadCursor(nullptr, IDC_ARROW);

    CString shapesStr;
    CString layoutStr;
    shapesStr.LoadString(IDS_STRING_SHAPES);
    layoutStr.LoadString(IDS_STRING_LAYOUT);

    m_ctrlTab.InsertItem(0, shapesStr);
    m_ctrlTab.InsertItem(1, layoutStr);
    m_ctrlTab.SetCurSel(tabMode);

    draggingOn = 0;

    if (!saImageListLoaded)
    {
        saImageList.Create(IDB_SCREENANNOTATIONS_SMALL, 16, 10, RGB(255, 255, 255));
        saImageListLoaded = 1;
        m_ctrlList.SetImageList(&saImageList, LVSIL_SMALL);
    }

    return bResult; // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CScreenAnnotationsDlg::OnContextMenu(CWnd * /*pWnd*/, CPoint /*point*/)
{
    // TODO: Add your message handler code here
}

void CScreenAnnotationsDlg::OnRclickList1(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
    int nItem = -1;
    int numitems = m_ctrlList.GetItemCount();

    for (int i = 0; i < numitems; i++)
    {
        int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
        if (itemstate & LVIS_SELECTED)
        {
            nItem = i;
        }
    }
    CMenu menu;
    int menuToLoad = IDR_CONTEXTEDITOBJ;

    POINT point;
    GetCursorPos(&point);

    // Layout Mode
    if (tabMode == modeLayout)
    {
        menuToLoad = IDR_CONTEXTEDITLAYOUT;

        if (menu.LoadMenu(menuToLoad))
        {
            CMenu *pPopup = menu.GetSubMenu(0);
            ASSERT(pPopup != nullptr);

            if (layoutIsCopied)
            {
                pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_ENABLED | MF_BYCOMMAND);
            }
            else
            {
                pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            }

            if (nItem < 0)
            {
                // item not selected
                pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            }
            else
            {
                pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT, MF_ENABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE, MF_ENABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_ENABLED | MF_BYCOMMAND);
                pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_ENABLED | MF_BYCOMMAND);
            }
            auto max = ListManager.displayArray.GetSize();
            bool bIsEdited = AreWindowsEdited();
            if (bIsEdited || (max <= 0))
            {
                pPopup->EnableMenuItem(ID_EDITLAYOUT_SAVELAYOUT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            }
            else
            {
                pPopup->EnableMenuItem(ID_EDITLAYOUT_SAVELAYOUT, MF_ENABLED | MF_BYCOMMAND);
            }

            if (bIsEdited)
            {
                pPopup->EnableMenuItem(ID_EDITLAYOUT_OPENLAYOUT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            }

            // route commands through main window
            pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
        } // End loadMenu Successful

        *pResult = 0;
        return;
    } // End Layout Mode

    // Shape Mode
    if (menu.LoadMenu(menuToLoad))
    {
        CMenu *pPopup = menu.GetSubMenu(0);
        ASSERT(pPopup != nullptr);

        if ((tabMode == modeLayout) || (nItem < 0) || (newObjectOn))
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE, MF_ENABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_ENABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_ENABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_TESTEDIT, MF_ENABLED | MF_BYCOMMAND);
        }

        if ((objectIsCopied) && (tabMode == modeShape))
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_ENABLED | MF_BYCOMMAND);
        }
        else if ((layoutIsCopied) && (tabMode == modeLayout))
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }

        if (nItem < 0)
        {
            // item not selected
            pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE, MF_ENABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_ENABLED | MF_BYCOMMAND);
        }

        if (newObjectOn)
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_NEWOBJECT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);

            pPopup->EnableMenuItem(ID_EDITOBJ_REMOVE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_COPY, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);

            pPopup->EnableMenuItem(ID_EDITOBJ_EDITIMAGE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTEXT, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
            pPopup->EnableMenuItem(ID_EDITOBJ_EDITTRANSPARENCY, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopup->EnableMenuItem(ID_EDITOBJ_NEWOBJECT, MF_ENABLED | MF_BYCOMMAND);

            // pPopup->EnableMenuItem(ID_EDITOBJ_LIBRARY_NEWLIBRARY,MF_ENABLED|MF_BYCOMMAND);
            // pPopup->EnableMenuItem(ID_EDITOBJ_LIBRARY_OPENLIBRARY,MF_ENABLED|MF_BYCOMMAND);
        }
        // route commands through main window
        pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
    }
    *pResult = 0;
}

void CScreenAnnotationsDlg::CloseAllWindows(int wantDelete)
{
    auto max = ListManager.displayArray.GetSize();
    CTransparentWnd *itemWnd = nullptr;
    for (auto i = max - 1; i >= 0; i--)
    {
        itemWnd = ListManager.displayArray[i];
        if (itemWnd)
        {
            if (itemWnd->TrackingOn() || itemWnd->EditTransOn() || itemWnd->EditImageOn())
            {
                // Do not remove, the item is undergoing editing
            }
            else
            {
                itemWnd->ShowWindow(SW_HIDE);
                if (wantDelete)
                {
                    ListManager.RemoveDisplayArray(itemWnd, 1);
                }
            }
        }
    }
}

void CScreenAnnotationsDlg::OnEditobjNewobject()
{
    newObjectOn = 1;

    CNewShapeDlg newshapeDlg;
    if (newshapeDlg.DoModal() == IDOK)
    {
        CTransparentWnd *newWnd;

        // TODO, Possible memory leak, where is the delete operation of the new below done?
        newWnd = new CTransparentWnd;

        int x = (rand() % 100) + 100;
        int y = (rand() % 100) + 100;
        CRect rect;
        if (g_iImageType == 0)
        {
            rect.left = x;
            rect.top = y;
            rect.right = rect.left + g_iNewShapeWidth - 1;
            rect.bottom = rect.top + g_iNewShapeHeight - 1;
            newWnd->TextString(g_strNewShapeText);
            newWnd->ShapeString(g_shapeStr);
            newWnd->CreateTransparent(g_shapeStr, rect, nullptr);
        }
        else
        {
            rect.left = x;
            rect.top = y;
            rect.right = x + 100;
            rect.bottom = y + 100;
            newWnd->TextString(g_strNewShapeText);
            newWnd->ShapeString(g_shapeStr);
            newWnd->CreateTransparent(g_shapeStr, rect, g_strImageFilename, 1);
        }

        if (newWnd)
        {
            CTransparentWnd *cloneWnd = newWnd->Clone(0, 0);
            ListManager.AddDisplayArray(cloneWnd);

            cloneWnd->ShowWindow(SW_SHOW);
            cloneWnd->UpdateWindow();

            // Use fill info for items, the wnd*, the text, the icons...etc
            TabSelectShapeMode(1);
            if (tabMode == modeShape)
            {
                // A shape window can never be shown....else crash...
                ListManager.AddShapeArray(newWnd);
                int nItem = m_ctrlList.GetItemCount();
                int nItemMod = modeShape;

                CString insstr = cloneWnd->ShapeString();
                int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0,
                                                nItemMod, newWnd->UniqueID());
                VERIFY(-1 != ins); // TODO: Handle error
            }
        }
    }
    newObjectOn = 0;
}

void CScreenAnnotationsDlg::OnEditobjEditimage()
{
    CTransparentWnd *editWnd = LocateWndFromShapeList();
    editWnd->SaveMethod(saveMethodReplace);
    editWnd->ShowWindow(SW_RESTORE);

    editWnd->EditImage();

    editWnd->SaveMethod(saveMethodNew);
    SaveShapeReplace(editWnd);
}

void CScreenAnnotationsDlg::OnEditobjEdittext()
{
    CTransparentWnd *editWnd = LocateWndFromShapeList();
    editWnd->SaveMethod(saveMethodReplace);
    editWnd->ShowWindow(SW_RESTORE);

    editWnd->EditText();

    editWnd->SaveMethod(saveMethodNew);
    SaveShapeReplace(editWnd);
}

void CScreenAnnotationsDlg::SaveLayoutNew()
{
    // set to layout mode for tabctrl
    TabSelectLayoutMode(1);

    CArray<CTransparentWnd *, CTransparentWnd *> *cloneArray = ListManager.CloneDisplayArray();
    if (cloneArray)
    {
        CLayoutList *newLayout = nullptr;

        // TODO, Possible memory leak, where is the delete operation of the new below done?
        newLayout = new CLayoutList;
        if (newLayout)
        {
            newLayout->layoutArrayPtr = cloneArray;

            CString layoutIntStr;
            layoutIntStr.Format(_T("%d"), iLayoutNameInt);
            newLayout->layoutName = g_strLayoutName + layoutIntStr;
            iLayoutNameInt++;
            if (iLayoutNameInt > 2147483647) // TODO; No Magic Numbers
                iLayoutNameInt = 1;

            ListManager.AddLayoutArray(newLayout);
            int nItem = m_ctrlList.GetItemCount();
            int nItemMod = modeLayout; // refering to layout images
            CString insstr = newLayout->layoutName;
            int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod,
                                            newLayout->uniqueID);
            VERIFY(-1 != ins); // TODO: Handle error

            m_ctrlList.EnsureVisible(nItem, FALSE);
            m_ctrlList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

void CScreenAnnotationsDlg::SaveShapeNew(CTransparentWnd *newWnd)
{
    // set to shape mode for tabctrl
    TabSelectShapeMode(1);

    if (newWnd)
    {
        CTransparentWnd *cloneWnd = newWnd->Clone(0, 0);
        ListManager.AddShapeArray(cloneWnd);

        int nItem = m_ctrlList.GetItemCount();
        int nItemMod = modeShape;

        CString shapeStr;
        shapeStr.Format(_T("%d"), iShapeNameInt);
        iShapeNameInt++;
        if (iShapeNameInt > 2147483600) // assume int32
            iShapeNameInt = 1;

        shapeStr = g_shapeName + shapeStr;
        CString insstr = shapeStr;

        insstr.TrimLeft();
        insstr.TrimRight();
        if (insstr == "")
        {
            insstr = cloneWnd->ShapeString();
        }
        else
        {
            int val = insstr.Find('\n');
            if (val > 0)
            {
                insstr = insstr.Left(val - 1);
                insstr = insstr + " ...";
            }
        }
        cloneWnd->ShapeString(insstr);
        int ins =
            m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM, nItem, LPCTSTR(insstr), 0, 0, nItemMod, cloneWnd->UniqueID());
        VERIFY(-1 != ins); // TODO: Handle error

        m_ctrlList.EnsureVisible(nItem, FALSE);
        m_ctrlList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
    }
}

void CScreenAnnotationsDlg::SaveShapeReplace(CTransparentWnd *newWnd)
{
    // set to shape mode for tabctrl
    TabSelectShapeMode(1);

    // assume the newWnd is the window that is edited
    if (newWnd)
    {
        newWnd->ShowWindow(SW_HIDE);
        // listctrl item is not affected because the shapestr and position and lParam/uniqueID remains the same
    }
}

void CScreenAnnotationsDlg::OnEditobjEdittransparency()
{
    CTransparentWnd *editWnd = LocateWndFromShapeList();
    editWnd->SaveMethod(saveMethodReplace);
    editWnd->ShowWindow(SW_RESTORE);

    editWnd->EditTransparency();

    editWnd->SaveMethod(saveMethodNew);
    SaveShapeReplace(editWnd);
}

void CScreenAnnotationsDlg::OnEditobjCopy()
{
    if (tabMode == modeShape)
    {
        CTransparentWnd *newWnd = LocateWndFromShapeList();

        if (newWnd)
        {
            if (objectIsCopied)
            {
                if (wndCopied)
                {
                    wndCopied->ShowWindow(SW_HIDE);

                    wndCopied->DestroyWindow();
                    delete wndCopied;

                    wndCopied = nullptr;
                    objectIsCopied = 0;
                }
            }
            wndCopied = newWnd->Clone(0, 0);

            if (wndCopied)
            {
                objectIsCopied = 1;
            }
            else
            {
                objectIsCopied = 0;
            }
        }
    }
    else if (tabMode == modeLayout)
    {
        int nItem = -1;
        int numitems = m_ctrlList.GetItemCount();

        for (int i = 0; i < numitems; i++)
        {
            // GetItemState( int nItem, LVIS_SELECTED )
            int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
            if (itemstate & LVIS_SELECTED)
            {
                nItem = i;
            }
        }
        CLayoutList *newLayout = nullptr;
        if ((nItem > -1) && (nItem < numitems))
        {
            CLayoutList *itemLayout = nullptr;
            itemLayout = LocateLayoutFromItem(nItem);

            newLayout = ListManager.CloneLayout(itemLayout);
        }
        if (newLayout)
        {
            if (layoutCopied)
            {
                ListManager.DestroyLayout(layoutCopied);
                layoutCopied = nullptr;
                layoutIsCopied = 0;
            }
            layoutCopied = newLayout;

            if (layoutCopied)
            {
                layoutIsCopied = 1;
            }
            else
            {
                layoutIsCopied = 0;
            }
        }
    }
}

void CScreenAnnotationsDlg::OnEditobjPaste()
{
    if ((tabMode == modeShape) && (objectIsCopied) && (wndCopied))
    {
        CTransparentWnd *cloneWnd = wndCopied->Clone(0, 0);
        ListManager.AddShapeArray(cloneWnd);

        int nItem = m_ctrlList.GetItemCount();
        int nItemMod = modeShape;
        int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(cloneWnd->ShapeString()), 0,
                                        0, nItemMod, cloneWnd->UniqueID());
        VERIFY(-1 != ins); // TODO: Handle error
        m_ctrlList.EnsureVisible(nItem, FALSE);
        m_ctrlList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
    }
    else if ((tabMode == modeLayout) && (layoutIsCopied) && (layoutCopied))
    {
        CLayoutList *cloneLayout = ListManager.CloneLayout(layoutCopied);
        if (cloneLayout)
        {
            ListManager.AddLayoutArray(cloneLayout);

            int nItem = m_ctrlList.GetItemCount();
            int nItemMod = modeLayout;
            int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem,
                                            LPCTSTR(cloneLayout->layoutName), 0, 0, nItemMod, cloneLayout->uniqueID);
            VERIFY(-1 != ins); // TODO: Handle error
            m_ctrlList.EnsureVisible(nItem, FALSE);
            m_ctrlList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

void CScreenAnnotationsDlg::OnEditobjRemove()
{
    int nItem = -1;
    int numitems = m_ctrlList.GetItemCount();

    for (int i = 0; i < numitems; i++)
    {
        // GetItemState( int nItem, LVIS_SELECTED )
        int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
        if (itemstate & LVIS_SELECTED)
        {
            nItem = i;
        }
    }
    CTransparentWnd *newWnd = nullptr;
    CLayoutList *newLayout = nullptr;
    if (tabMode == modeShape)
    {
        if ((nItem > -1) && (nItem < numitems))
        {
            newWnd = LocateWndFromItem(nItem);
        }
        if (newWnd)
        {
            ListManager.RemoveShapeArray(newWnd, 1);
            m_ctrlList.DeleteItem(nItem);
        }
    }
    else if (tabMode == modeLayout)
    {
        if ((nItem > -1) && (nItem < numitems))
        {
            newLayout = LocateLayoutFromItem(nItem);
        }
        if (newLayout)
        {
            ListManager.RemoveLayoutArray(newLayout, 1);
            m_ctrlList.DeleteItem(nItem);
        }
    }
}

void CScreenAnnotationsDlg::OnEditobjTestedit()
{
    int nItem = -1;
    int numitems = m_ctrlList.GetItemCount();

    for (int i = 0; i < numitems; i++)
    {
        int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
        if (itemstate & LVIS_SELECTED)
        {
            nItem = i;
        }
    }

    if ((nItem > -1) && (nItem < numitems))
    {
        m_bEditingLabelOn = true;
        m_ctrlList.EditLabel(nItem);
    }
}

void CScreenAnnotationsDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CScreenAnnotationsDlg::OnCancel()
{
    if (!m_bEditingLabelOn)
    {
        return;
    }
    CDialog::OnCancel();
}

void CScreenAnnotationsDlg::OnEndlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_bEditingLabelOn = false;
    LV_DISPINFO *pDispInfo = (LV_DISPINFO *)pNMHDR;

    if ((pDispInfo->item).pszText == nullptr)
    {
        pResult = FALSE;
        return;
    }

    if (tabMode == modeShape)
    {
        // Get Transparent Window
        // Change the shapeStr of the Transparent Window
        CTransparentWnd *newWnd = LocateWndFromShapeList();

        if (newWnd)
        {
            newWnd->ShapeString((pDispInfo->item).pszText);
            if (newWnd->ShapeString() != g_shapeStr)
            {
                // shape name has been changed, reset counter to 1
                g_shapeName = newWnd->ShapeString();

                // a better method is to extract the trailing number from shapestr and use it as number for
                // iShapeNameInt
                AdjustShapeName(g_shapeName);
            }
            else
            {
                iShapeNameInt++;
                if (iShapeNameInt > 2147483600) // assume int32
                    iShapeNameInt = 1;
            }
            *pResult = TRUE;
            return;
        }
    }
    else if (tabMode == modeLayout)
    {
        CLayoutList *itemLayout = nullptr;

        int nItem = -1;
        int numitems = m_ctrlList.GetItemCount();

        for (int i = 0; i < numitems; i++)
        {
            int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
            if (itemstate & LVIS_SELECTED)
            {
                nItem = i;
            }
        }
        if ((nItem > -1) && (nItem < numitems))
        {
            itemLayout = LocateLayoutFromItem(nItem);
        }
        if (itemLayout)
        {
            itemLayout->layoutName = (pDispInfo->item).pszText;
            if (itemLayout->layoutName != g_strLayoutName)
            {
                // shape name has been changed, reset counter to 1
                g_strLayoutName = itemLayout->layoutName;

                // a better method is to extract the trailing number from shapestr and use it as number for
                // iLayoutNameInt
                AdjustLayoutName(g_strLayoutName);
            }
            else
            {
                iLayoutNameInt++;
                if (iLayoutNameInt > 2147483600) // assume int32
                    iLayoutNameInt = 1;
            }
            *pResult = TRUE;
            return;
        }
    }
}

void CScreenAnnotationsDlg::OnDblclkList1(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
    if (tabMode == modeShape)
    {
        int x = (rand() % 200) - 100;
        int y = (rand() % 200) - 100;
        InstantiateWnd(x, y);
    }
    else if (tabMode == modeLayout)
    {
        InstantiateLayout();
    }
    *pResult = 0;
}

void CScreenAnnotationsDlg::InstantiateWnd(int x, int y)
{
    if (tabMode == modeShape)
    {
        CTransparentWnd *newWnd = LocateWndFromShapeList();

        if (newWnd)
        {
            CTransparentWnd *cloneWnd = newWnd->Clone(x, y);
            ListManager.AddDisplayArray(cloneWnd);

            cloneWnd->InvalidateRegion();
            cloneWnd->InvalidateTransparency();
            cloneWnd->ShowWindow(SW_SHOW);
            cloneWnd->UpdateWindow();
        }
    }
}

CTransparentWnd *CScreenAnnotationsDlg::LocateWndFromShapeList()
{
    CTransparentWnd *newWnd = nullptr;

    int nItem = -1;
    int numitems = m_ctrlList.GetItemCount();

    for (int i = 0; i < numitems; i++)
    {
        int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
        if (itemstate & LVIS_SELECTED)
        {
            nItem = i;
        }
    }
    if ((nItem > -1) && (nItem < numitems))
    {
        newWnd = LocateWndFromItem(nItem);
    }
    return newWnd;
}

CTransparentWnd *CScreenAnnotationsDlg::LocateWndFromItem(int nItem)
{
    CTransparentWnd *newWnd = nullptr;
    long WndID = -1;

    LV_ITEM searchItem;
    searchItem.mask = LVIF_PARAM;
    searchItem.iItem = nItem;
    m_ctrlList.GetItem(&searchItem);

    WndID = searchItem.lParam;

    auto max = ListManager.shapeArray.GetSize();
    CTransparentWnd *itemWnd = nullptr;
    for (auto i = 0; i < max; i++)
    {
        itemWnd = ListManager.shapeArray[i];
        if (itemWnd)
        {
            if (itemWnd->UniqueID() == WndID)
            {
                newWnd = itemWnd;
                break; // allow only one removal
            }
        }
    }
    return newWnd;
}

void CScreenAnnotationsDlg::OnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LV_KEYDOWN *pLVKeyDow = (LV_KEYDOWN *)pNMHDR;

    if (pLVKeyDow->wVKey == VK_F2)
    {
        OnEditobjTestedit();
    }
    *pResult = 0;
}

void CScreenAnnotationsDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if (objectIsCopied)
    {
        if (wndCopied)
        {
            wndCopied->ShowWindow(SW_HIDE);

            wndCopied->DestroyWindow();
            delete wndCopied;

            wndCopied = nullptr;
            objectIsCopied = 0;
        }
    }
    if (layoutIsCopied)
    {
        if (layoutCopied)
        {
            ListManager.DestroyLayout(layoutCopied);
        }
    }
}

void CScreenAnnotationsDlg::OnBegindragList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
    iDragIndex = pNMListView->iItem;
    draggingOn = 1;
    SetCursor(m_hCursorDrag);
    SetCapture();

    *pResult = 0;
}

void CScreenAnnotationsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (draggingOn)
    {
        SetCursor(m_hCursorDrag);
    }
    CDialog::OnMouseMove(nFlags, point);
}

void CScreenAnnotationsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (draggingOn)
    {
        ReleaseCapture();
        draggingOn = 0;
        SetCursor(m_hCursorArrow);

        CPoint pt(point);    // Get current mouse coordinates
        ClientToScreen(&pt); // Convert to screen coordinates
        CWnd *pDropWnd = WindowFromPoint(pt);

        if (tabMode == modeShape)
        {
            if (pDropWnd != &m_ctrlList)
            {
                CTransparentWnd *newWnd = LocateWndFromShapeList();

                if (newWnd)
                {
                    GetCursorPos(&pt);

                    int wx = (newWnd->RectWnd().Width()) / 2;
                    int hx = (newWnd->RectWnd().Height()) / 2;
                    CTransparentWnd *cloneWnd = newWnd->CloneByPos(pt.x - wx, pt.y - hx);
                    ListManager.AddDisplayArray(cloneWnd);

                    cloneWnd->InvalidateRegion();
                    cloneWnd->InvalidateTransparency();
                    cloneWnd->ShowWindow(SW_SHOW);
                    cloneWnd->UpdateWindow();
                }
            }
            else
            {
                // Locate Selected Item
                int nItem = -1;
                int numitems = m_ctrlList.GetItemCount();

                for (int i = 0; i < numitems; i++)
                {
                    int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
                    if (itemstate & LVIS_SELECTED)
                    {
                        nItem = i;
                    }
                }
                if ((nItem > -1) && (nItem < numitems))
                {
                    CPoint ptx;
                    GetCursorPos(&ptx);
                    int hotspotx = 13;
                    int hotspoty = 10;
                    ptx.x += hotspotx;
                    ptx.y += hotspoty;
                    m_ctrlList.ScreenToClient(&ptx);
                    UINT lvFlags = LVHT_ONITEM;
                    int targetItem = m_ctrlList.HitTest(ptx, &lvFlags);

                    if ((targetItem >= 0) && (targetItem != nItem))
                    {
                        // int nItemMod = modeShape;

                        LV_ITEM swapItem, swapItem2;
                        swapItem.mask = LVIF_PARAM;
                        swapItem.iItem = nItem;
                        m_ctrlList.GetItem(&swapItem);

                        swapItem2.mask = LVIF_PARAM;
                        swapItem2.iItem = targetItem;
                        m_ctrlList.GetItem(&swapItem2);

                        ListManager.SwapShapeArray(swapItem.lParam, swapItem2.lParam);
                        RefreshShapeList();
                    }
                }
            }
        }
        else if (tabMode == modeLayout)
        {
            if (pDropWnd != &m_ctrlList)
            {
                InstantiateLayout();
            }
            else
            {
                // Locate Selected Item
                int nItem = -1;
                int numitems = m_ctrlList.GetItemCount();

                for (int i = 0; i < numitems; i++)
                {
                    int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
                    if (itemstate & LVIS_SELECTED)
                    {
                        nItem = i;
                    }
                }
                if ((-1 < nItem) && (nItem < numitems))
                {
                    // back here
                    CPoint ptx;
                    GetCursorPos(&ptx);
                    int hotspotx = 13;
                    int hotspoty = 10;
                    ptx.Offset(hotspotx, hotspoty);
                    m_ctrlList.ScreenToClient(&ptx);
                    UINT lvFlags = LVHT_ONITEM;
                    int targetItem = m_ctrlList.HitTest(ptx, &lvFlags);
                    if ((0 <= targetItem) && (targetItem != nItem))
                    {
                        // int nItemMod = modeLayout;

                        LV_ITEM swapItem, swapItem2;
                        swapItem.mask = LVIF_PARAM;
                        swapItem.iItem = nItem;
                        m_ctrlList.GetItem(&swapItem);

                        swapItem2.mask = LVIF_PARAM;
                        swapItem2.iItem = targetItem;
                        m_ctrlList.GetItem(&swapItem2);

                        ListManager.SwapLayoutArray(swapItem.lParam, swapItem2.lParam);
                        RefreshLayoutList();
                    }
                }
            }
        }
    }
    CDialog::OnLButtonUp(nFlags, point);
}

void CScreenAnnotationsDlg::RefreshShapeList()
{
    // tabmode must be set to shapeMode before calling this
    m_ctrlList.DeleteAllItems();

    auto max = ListManager.shapeArray.GetSize();
    for (auto i = 0; i < max; i++)
    {
        CTransparentWnd *pItemWnd = ListManager.shapeArray[i];
        if (pItemWnd)
        {
            int nItem = m_ctrlList.GetItemCount();
            int nItemMod = modeShape; // refering to shape images
            CString insstr = pItemWnd->ShapeString();
            int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod,
                                            pItemWnd->UniqueID());
            VERIFY(-1 != ins); // TODO: Handle error
        }
    }
}

void CScreenAnnotationsDlg::RefreshLayoutList()
{
    // tabmode must be layoutMode before calling this
    m_ctrlList.DeleteAllItems();

    auto max = ListManager.layoutArray.GetSize();
    for (auto i = decltype(max)(0); i < max; i++)
    {
        CLayoutList *itemLayout = ListManager.layoutArray[i];
        if (itemLayout)
        {
            int nItem = m_ctrlList.GetItemCount();
            int nItemMod = modeLayout; // refering to layout images
            CString insstr = itemLayout->layoutName;
            int ins = m_ctrlList.InsertItem(LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE, nItem, LPCTSTR(insstr), 0, 0, nItemMod,
                                            itemLayout->uniqueID);
            VERIFY(-1 != ins); // TODO: Handle error
        }
    }
}

void CScreenAnnotationsDlg::TabSelectShapeMode(int updateTab)
{
    if (tabMode == modeLayout)
    {
        RefreshShapeList();

        tabMode = modeShape;

        if (updateTab)
        {
            m_ctrlTab.SetCurSel(tabMode);
        }
    }
}

void CScreenAnnotationsDlg::TabSelectLayoutMode(int updateTab)
{
    if (tabMode == modeShape)
    {
        RefreshLayoutList();

        tabMode = modeLayout;

        if (updateTab)
        {
            m_ctrlTab.SetCurSel(tabMode);
        }
    }
}

void CScreenAnnotationsDlg::OnSelchangeTab1(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
    int sel = m_ctrlTab.GetCurSel();
    UpdateTabCtrl(sel);

    *pResult = 0;
}

void CScreenAnnotationsDlg::UpdateTabCtrl(int sel)
{
    if (sel == modeShape)
    {
        TabSelectShapeMode(0);
    }
    else
        TabSelectLayoutMode(0);
}

void CScreenAnnotationsDlg::OnEditlayoutSavelayout()
{
    SaveLayoutNew();
}

void CScreenAnnotationsDlg::OnEditlayoutOpenlayout()
{
    InstantiateLayout();
}

void CScreenAnnotationsDlg::OnEditlayoutCloseallobjects()
{
    CloseAllWindows(1);
}

void CScreenAnnotationsDlg::OnEditlayoutLayoutshortcuts()
{
    if (!g_keySCOpened)
    {
        g_keySCOpened = 1;
        CKeyshortcutsDlg kscDlg;
        kscDlg.DoModal();
        g_keySCOpened = 0;

        VERIFY(7 == SetAdjustHotKeys());
    }
}
CLayoutList *CScreenAnnotationsDlg::LocateLayoutFromItem(int nItem)
{
    CLayoutList *returnLayout = nullptr;

    long LayoutID = -1;

    LV_ITEM searchItem;
    searchItem.mask = LVIF_PARAM;
    searchItem.iItem = nItem;
    m_ctrlList.GetItem(&searchItem);
    LayoutID = searchItem.lParam;

    CLayoutList *itemLayout = nullptr;
    auto max = ListManager.layoutArray.GetSize();
    for (auto i = max - 1; i >= 0; i--)
    {
        itemLayout = ListManager.layoutArray[i];
        if (itemLayout)
        {
            if (itemLayout->uniqueID == LayoutID)
            {
                returnLayout = itemLayout;
                break; // allow only one removal
            }
        }
    }
    return returnLayout;
}

void CScreenAnnotationsDlg::InstantiateLayout()
{
    if (tabMode == modeLayout)
    {
        // Locate layout
        CLayoutList *itemLayout = nullptr;

        int nItem = -1;
        int numitems = m_ctrlList.GetItemCount();

        for (int i = 0; i < numitems; i++)
        {
            int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
            if (itemstate & LVIS_SELECTED)
            {
                nItem = i;
            }
        }
        if ((nItem > -1) && (nItem < numitems))
        {
            itemLayout = LocateLayoutFromItem(nItem);
        }
        if (itemLayout)
        {
            if (itemLayout->layoutArrayPtr)
            {
                auto max = itemLayout->layoutArrayPtr->GetSize();

                CTransparentWnd *itemWnd = nullptr;
                // for (int i=max-1;i>=0; i--)
                for (INT_PTR i = 0; i < max; i++)
                {
                    itemWnd = (*(itemLayout->layoutArrayPtr))[i];
                    if (itemWnd)
                    {
                        CTransparentWnd *cloneWnd = itemWnd->Clone(0, 0);
                        ListManager.AddDisplayArray(cloneWnd);

                        cloneWnd->UpdateWindow();
                        cloneWnd->InvalidateRegion();
                        cloneWnd->InvalidateTransparency();
                        cloneWnd->ShowWindow(SW_SHOW);
                    }
                }
            }
        }
    }
}

void CScreenAnnotationsDlg::OnEditobjLibraryCloselibrary()
{
    CString m_newfile;

    static const TCHAR *szFilter = _T("Shape Files (*.shapes)|*.shapes||");
    static const TCHAR *szTitle = _T("Save Shapes");

    CFileDialog fdlg(FALSE, _T("*.shapes"), _T("*.shapes"), OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT, szFilter, this);
    fdlg.m_ofn.lpstrTitle = szTitle;
    fdlg.m_ofn.lpstrDefExt = _T("shapes");

    if (fdlg.DoModal() == IDOK)
    {
        m_newfile = fdlg.GetPathName();

        CString compareStr = GetProgPath() + "\\CamShapes.ini";
        CString teststr = m_newfile;
        compareStr.MakeLower();
        teststr.MakeLower();
        if (compareStr == teststr)
        {
            MessageOut(nullptr, IDS_STRINGCANNOTSAVEO, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);

            return;
        }
        ListManager.SaveShapeArray(m_newfile);
    }
}

void CScreenAnnotationsDlg::OnEditobjLibraryOpenlibrary()
{
    static const TCHAR *szFilter = _T("Shape Files (*.shapes)|*.shapes||");
    static const TCHAR *szTitle = _T("Load Shapes");

    CFileDialog fdlg(TRUE, _T("*.shapes"), _T("*.shapes"), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilter, this);
    fdlg.m_ofn.lpstrTitle = szTitle;
    if (IDOK == fdlg.DoModal())
    {
        CString filename = fdlg.GetPathName();
        ListManager.LoadShapeArray(filename);
        RefreshShapeList();
    }
}

void CScreenAnnotationsDlg::OnEditobjLibraryNewlibrary()
{
    int ret = MessageOut(nullptr, IDS_STRINGSAVECL, IDS_STRING_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (ret == IDYES)
    {
        OnEditobjLibraryCloselibrary();
    }
    else if (ret == IDCANCEL)
        return;

    ListManager.FreeShapeArray();
    RefreshShapeList();
}

void CScreenAnnotationsDlg::OnEditlayoutLibraryNewlibrary()
{
    int ret = MessageOut(nullptr, IDS_STRINGSAVECLLA, IDS_STRING_NOTE, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (ret == IDYES)
    {
        OnEditlayoutLibraryCloselibrary();
    }
    else if (ret == IDCANCEL)
        return;

    ListManager.FreeLayoutArray();
    RefreshLayoutList();
}

void CScreenAnnotationsDlg::OnEditlayoutLibraryOpenlibrary()
{
    CString filename;

    static const TCHAR *szFilter = _T("Layout Files (*.layout)|*.layout||");
    static const TCHAR *szTitle = _T("Load Layout");

    CFileDialog fdlg(TRUE, _T("*.layout;*.lay"), _T("*.layout;*.lay"), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilter, this);
    fdlg.m_ofn.lpstrTitle = szTitle;

    if (fdlg.DoModal() == IDOK)
    {
        filename = fdlg.GetPathName();
        ListManager.LoadLayout(filename);
        RefreshLayoutList();
    }
}

void CScreenAnnotationsDlg::OnEditlayoutLibraryCloselibrary()
{
    CString m_newfile;

    static const TCHAR *szFilter = _T("Layout Files (*.layout)|*.layout||");
    static const TCHAR *szTitle = _T("Save Layout");

    CFileDialog fdlg(FALSE, _T("*.layout"), _T("*.layout"), OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT, szFilter, this);
    fdlg.m_ofn.lpstrTitle = szTitle;
    fdlg.m_ofn.lpstrDefExt = _T("layout");

    if (fdlg.DoModal() == IDOK)
    {
        m_newfile = fdlg.GetPathName();

        CString compareStr = GetAppDataPath() + "\\CamLayout.ini";
        CString teststr = m_newfile;
        compareStr.MakeLower();
        teststr.MakeLower();
        if (compareStr == teststr)
        {
            // MessageBox("You cannot save over the default layout file.","Note",MB_OK | MB_ICONEXCLAMATION);
            MessageOut(nullptr, IDS_STRINGcANNOTSAVEOL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        ListManager.SaveLayout(m_newfile);
    }
}

void CScreenAnnotationsDlg::OnHelpHelp()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#ScreenAnn";

    //Openlink(helpScreenPath);
}

void CScreenAnnotationsDlg::OnOptionsClosescreenannotations()
{
    OnOK();
}

// partial
void CScreenAnnotationsDlg::InstantiateLayout(int nItem, int /*makeselect*/)
{
    TabSelectLayoutMode(1);

    if (tabMode == modeLayout)
    {
        // Locate layout
        CLayoutList *itemLayout = nullptr;

        int numitems = m_ctrlList.GetItemCount();

        if ((nItem > -1) && (nItem < numitems))
        {
            itemLayout = LocateLayoutFromItem(nItem);

            m_ctrlList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
            m_ctrlList.EnsureVisible(nItem, FALSE);
        }
        if (itemLayout)
        {
            // Close all others and instantiate
            CloseAllWindows(1);

            if (itemLayout->layoutArrayPtr)
            {
                auto max = itemLayout->layoutArrayPtr->GetSize();

                CTransparentWnd *itemWnd = nullptr;
                for (INT_PTR i = 0; i < max; i++)
                {
                    itemWnd = (*(itemLayout->layoutArrayPtr))[i];

                    if (itemWnd)
                    {
                        CTransparentWnd *cloneWnd = itemWnd->Clone(0, 0);
                        ListManager.AddDisplayArray(cloneWnd);
                        cloneWnd->UpdateWindow();
                        cloneWnd->InvalidateRegion();
                        cloneWnd->InvalidateTransparency();
                        cloneWnd->ShowWindow(SW_SHOW);
                    }
                }
            }
        }
    }
}

// GetLayoutListSelection
// Return the index of the selected m_ctrlList item or -1 if no selection
// m_ctrlList is a single selection list; so there is either a selection
// or not there isn't
int CScreenAnnotationsDlg::GetLayoutListSelection()
{
    POSITION pos = m_ctrlList.GetFirstSelectedItemPosition();
    return (pos != nullptr) ? m_ctrlList.GetNextSelectedItem(pos) : -1;
}

void CScreenAnnotationsDlg::OnNext()
{
    auto max = ListManager.layoutArray.GetSize();
    if (max <= 0)
        return;

    // Get Current selected
    int cursel = GetLayoutListSelection();
    g_iCurrentLayout = (cursel == -1) ? 0 : cursel + 1;
    if (max <= g_iCurrentLayout)
        g_iCurrentLayout = 0;

    InstantiateLayout(g_iCurrentLayout, 1);
}

void CScreenAnnotationsDlg::OnHelpShapes()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#Shape";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsInstantiatingashape()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#InstantiatingShapes";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsEditingashape()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#EditingShapes";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsEditingtext()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#EditText";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsEditingimage()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#EditImage";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsCreatinganewshape()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#NewShape";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsEditingtransparency()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#EditTrans";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsManagingshapes()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#ManagingShapes";
}

void CScreenAnnotationsDlg::OnHelpShapetopicsResizingshapes()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#Resize";
}

void CScreenAnnotationsDlg::OnHelpLayouts()
{
    CString progdir, helpScreenPath;
    progdir = GetProgPath();
    helpScreenPath = progdir + "\\help.htm#Layout";
}

void CScreenAnnotationsDlg::OnAnnSavelayout()
{
    SaveLayoutNew();
}

void CScreenAnnotationsDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

    auto max = ListManager.displayArray.GetSize();

    if (max <= 0)
    {
        pPopupMenu->EnableMenuItem(IDR_ANN_SAVELAYOUT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_CLOSEALLOBJECTS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
    }
    else
    {
        pPopupMenu->EnableMenuItem(IDR_ANN_SAVELAYOUT, MF_ENABLED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_CLOSEALLOBJECTS, MF_ENABLED | MF_BYCOMMAND);
    }
    int numitems = m_ctrlList.GetItemCount();
    int numsel = m_ctrlList.GetSelectedCount();
    if ((numitems <= 0) || (numsel <= 0))
    {
        pPopupMenu->EnableMenuItem(ID_OBJECTS_COPY, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_REMOVE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

        pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

        pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
    }
    else
    {
        pPopupMenu->EnableMenuItem(ID_OBJECTS_COPY, MF_ENABLED | MF_BYCOMMAND);
        pPopupMenu->EnableMenuItem(ID_OBJECTS_REMOVE, MF_ENABLED | MF_BYCOMMAND);

        if (tabMode == modeShape)
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT, MF_ENABLED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE, MF_ENABLED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME, MF_ENABLED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY, MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTEXT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITIMAGE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITTRANSAPRENCY, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
            pPopupMenu->EnableMenuItem(ID_OBJECTS_EDITNAME, MF_ENABLED | MF_BYCOMMAND);
        }
        // Enabling / Disabling Item Up / Down
        int nItem = -1;
        for (int i = 0; i < numitems; i++)
        {
            int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
            if (itemstate & LVIS_SELECTED)
            {
                nItem = i;
            }
        }
        if ((nItem > -1) && (nItem < numitems))
        {
            if (nItem > 0)
                pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP, MF_ENABLED | MF_BYCOMMAND);
            else
                pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMUP, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

            if (nItem < numitems - 1)
                pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN, MF_ENABLED | MF_BYCOMMAND);
            else
                pPopupMenu->EnableMenuItem(ID_OBJECTS_MOVEITEMDOWN, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
        }
    }
    if (tabMode == modeLayout)
    {
        if (layoutIsCopied)
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE, MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }
    }
    else if (tabMode == modeShape)
    {
        if (objectIsCopied)
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE, MF_ENABLED | MF_BYCOMMAND);
        }
        else
        {
            pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        }
    }
    else
        pPopupMenu->EnableMenuItem(ID_OBJECTS_PASTE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
}

void CScreenAnnotationsDlg::OnLibraryOpenshapelibrary()
{
    OnEditobjLibraryOpenlibrary();
    TabSelectShapeMode(1);
}

void CScreenAnnotationsDlg::OnLibrarySaveshapelibrary()
{
    OnEditobjLibraryCloselibrary();
}

void CScreenAnnotationsDlg::OnLibraryNewshapelibrary()
{
    OnEditobjLibraryNewlibrary();
    TabSelectShapeMode(1);
}

void CScreenAnnotationsDlg::OnLibraryNewlayoutlibrary()
{
    OnEditlayoutLibraryNewlibrary();
    TabSelectLayoutMode(1);
}

void CScreenAnnotationsDlg::OnLibraryOpenlayoutlibrary()
{
    OnEditlayoutLibraryOpenlibrary();
    TabSelectLayoutMode(1);
}

void CScreenAnnotationsDlg::OnLibrarySavelayoutlibrary()
{
    OnEditlayoutLibraryCloselibrary();
}

void CScreenAnnotationsDlg::OnObjectsCopy()
{
    OnEditobjCopy();
}

void CScreenAnnotationsDlg::OnObjectsPaste()
{
    OnEditobjPaste();
}

void CScreenAnnotationsDlg::OnObjectsRemove()
{
    OnEditobjRemove();
}

int CScreenAnnotationsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void CScreenAnnotationsDlg::OnObjectsEdittext()
{
    OnEditobjEdittext();
}

void CScreenAnnotationsDlg::OnObjectsEditimage()
{
    OnEditobjEditimage();
}

void CScreenAnnotationsDlg::OnObjectsEditname()
{
    OnEditobjTestedit();
}

void CScreenAnnotationsDlg::OnObjectsEdittransaprency()
{
    OnEditobjEdittransparency();
}

void CScreenAnnotationsDlg::OnObjectsCloseallobjects()
{
    OnEditlayoutCloseallobjects();
}

void CScreenAnnotationsDlg::OnLibraryInsertshapelibaray()
{
    CString filename;

    static const TCHAR *szFilter = _T("Shape Files (*.shapes)|*.shapes||");
    static const TCHAR *szTitle = _T("Insert Shapes From File");

    CFileDialog fdlg(TRUE, _T("*.shapes"), _T("*.shapes"), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilter, this);
    fdlg.m_ofn.lpstrTitle = szTitle;

    if (fdlg.DoModal() == IDOK)
    {
        filename = fdlg.GetPathName();
        ListManager.LoadShapeArray(filename, 0);

        RefreshShapeList();
    }
    TabSelectShapeMode(1);
}

void CScreenAnnotationsDlg::OnObjectsMoveitemup()
{
    MoveItem(0);
}

void CScreenAnnotationsDlg::OnObjectsMoveitemdown()
{
    MoveItem(1);
}

void CScreenAnnotationsDlg::MoveItem(int direction)
{
    // Locate Selected Item
    int nItem = -1;
    int numitems = m_ctrlList.GetItemCount();

    for (int i = 0; i < numitems; i++)
    {
        int itemstate = m_ctrlList.GetItemState(i, LVIS_SELECTED);
        if (itemstate & LVIS_SELECTED)
        {
            nItem = i;
        }
    }
    int newitem = 0;
    if ((nItem > -1) && (nItem < numitems))
    {
        if (direction == 0) // up
        {
            if (nItem > 0)
            {
                newitem = nItem - 1;
            }
            else
                return;
        }
        else if (direction == 1) // down
        {
            if (nItem < numitems - 1)
            {
                newitem = nItem + 1;
            }
            else
                return;
        }
        LV_ITEM swapItem, swapItem2;
        swapItem.mask = LVIF_PARAM;
        swapItem.iItem = nItem;
        m_ctrlList.GetItem(&swapItem);

        swapItem2.mask = LVIF_PARAM;
        swapItem2.iItem = newitem;
        m_ctrlList.GetItem(&swapItem2);

        if (tabMode == modeShape)
        {
            ListManager.SwapShapeArray(swapItem.lParam, swapItem2.lParam);
            RefreshShapeList();
        }
        else if (tabMode == modeLayout)
        {
            ListManager.SwapLayoutArray(swapItem.lParam, swapItem2.lParam);
            RefreshLayoutList();
        }
        if ((newitem > -1) && (newitem < numitems))
        {
            m_ctrlList.EnsureVisible(newitem, FALSE);
            m_ctrlList.SetItemState(newitem, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

void CScreenAnnotationsDlg::AdjustLayoutName(CString &layoutName)
{
    CString reconstructNum("");
    int numchar = layoutName.GetLength();
    for (int i = numchar - 1; i >= 0; i--)
    {
        int txchar = layoutName[i];
        if ((txchar >= '0') && (txchar <= '9'))
        {
            reconstructNum = reconstructNum + layoutName[i];
        }
        else
        {
            break;
        }
    }
    reconstructNum.MakeReverse();
    int xchar = reconstructNum.GetLength();
    if (xchar <= 0)
    {
        iLayoutNameInt = 1;
    }
    else
    {
        for (int j = 0; j < xchar; j++)
        {
            if (reconstructNum[j] == '0')
            {
                xchar--;
            }
            else
            {
                break;
            }
        }
        if ((xchar >= 0) && (xchar <= numchar))
        {
            int val = std::stoi(reconstructNum.GetString());
            if ((val >= 0) && (val < 100000))
            {
                iLayoutNameInt = val + 1;
                layoutName = layoutName.Left(numchar - xchar);
            }
            else
            {
                iLayoutNameInt = 1;
            }
        }
        else
        {
            iLayoutNameInt = 1;
        }
    }
}

void AdjustShapeName(CString &shapeName)
{
    CString reconstructNum("");
    int numchar = shapeName.GetLength();
    for (int i = numchar - 1; 0 <= i; --i)
    {
        int txchar = shapeName[i];
        if (('0' <= txchar) && (txchar <= '9'))
        {
            reconstructNum = reconstructNum + shapeName[i];
        }
        else
        {
            break;
        }
    }
    reconstructNum.MakeReverse();
    int xchar = reconstructNum.GetLength();
    if (xchar <= 0)
    {
        iShapeNameInt = 1;
    }
    else
    {
        for (int j = 0; j < xchar; j++)
        {
            if (reconstructNum[j] == '0')
            {
                xchar--;
            }
            else
            {
                break;
            }
        }
        if ((xchar >= 0) && (xchar <= numchar))
        {
            int val = std::stoi(reconstructNum.GetString());
            if ((val >= 0) && (val < 100000))
            {
                iShapeNameInt = val + 1;
                shapeName = shapeName.Left(numchar - xchar);
            }
            else
            {
                iShapeNameInt = 1;
            }
        }
        else
        {
            iShapeNameInt = 1;
        }
    }
}

// terminate early on first edited window
bool AreWindowsEdited()
{
    bool bisEdited = false;
    auto max = ListManager.displayArray.GetSize();
    for (auto i = max - 1; 0 <= i; --i)
    {
        CTransparentWnd *itemWnd = ListManager.displayArray[i];
        if (itemWnd)
        {
            bisEdited = (itemWnd->TrackingOn() || itemWnd->EditTransOn() || itemWnd->EditImageOn());
            if (bisEdited)
            {
                return bisEdited;
            }
        }
    }
    max = ListManager.shapeArray.GetSize();
    for (auto i = max - 1; 0 <= i; --i)
    {
        CTransparentWnd *itemWnd = ListManager.shapeArray[i];
        if (itemWnd)
        {
            bisEdited = (itemWnd->TrackingOn() || itemWnd->EditTransOn() || itemWnd->EditImageOn());
            if (bisEdited)
            {
                return bisEdited;
            }
        }
    }
    return bisEdited;
}
