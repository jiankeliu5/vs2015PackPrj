
// mfcexetestView.cpp : CmfcexetestView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "mfcexetest.h"
#endif

#include "mfcexetestDoc.h"
#include "mfcexetestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmfcexetestView

IMPLEMENT_DYNCREATE(CmfcexetestView, CView)

BEGIN_MESSAGE_MAP(CmfcexetestView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CmfcexetestView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CmfcexetestView ����/����

CmfcexetestView::CmfcexetestView()
{
	// TODO: �ڴ˴���ӹ������

}

CmfcexetestView::~CmfcexetestView()
{
}

BOOL CmfcexetestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CmfcexetestView ����

void CmfcexetestView::OnDraw(CDC* /*pDC*/)
{
	CmfcexetestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CmfcexetestView ��ӡ


void CmfcexetestView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CmfcexetestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CmfcexetestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CmfcexetestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CmfcexetestView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CmfcexetestView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CmfcexetestView ���

#ifdef _DEBUG
void CmfcexetestView::AssertValid() const
{
	CView::AssertValid();
}

void CmfcexetestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmfcexetestDoc* CmfcexetestView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmfcexetestDoc)));
	return (CmfcexetestDoc*)m_pDocument;
}
#endif //_DEBUG


// CmfcexetestView ��Ϣ�������
