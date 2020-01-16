#pragma once

// Office Word 2010 lib
// #if 0	//��������Ҫʾ�����ٴβųɹ�������ʹ�õ��������Ŀ����벻ͨ��
// #import "../Lib/Office2010/MSWORD.OLB" \
// 	no_namespace raw_interfaces_only \
// 	rename("FindText", "Word_FindText") \
// 	rename("Rectangle", "Word_Rectangle") \
// 	rename("ExitWindows","Word_ExitWindows")
// #else	//msword.tlh������#import���ɵ��ļ�
// #include "../Lib/Office2010/msword.tlh"
// #endif


//�����а�����ͷ�ļ�ע��import
// #import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\MSWORD.OLB" no_namespace
//���԰���Word2010�µ�����ͷ�ļ���Ŀǰֻ���������õ���ͷ�ļ�
#include "Word2010/CApplication.h"	// word�������
#include "Word2010/CDocuments.h"	// �ĵ�������
#include "Word2010/CDocument0.h"	// ����docx����
#include "Word2010/CSelection.h"	// �ö�������ڻ򴰸��еĵ�ǰ��ѡ���ݣ�ʹ�������
#include "Word2010/CCell.h"		// ���Ԫ��
#include "Word2010/CCells.h"	// ���Ԫ�񼯺�
#include "Word2010/CRange.h"	// �ö�������ĵ��е�һ��������Χ
#include "Word2010/CTable0.h"	// �������
#include "Word2010/CTables0.h"	// ��񼯺�
#include "Word2010/CFont0.h"	// ����
#include "Word2010/CParagraphs.h"	// ���伯��
#include "Word2010/CParagraphFormat.h"	// ������ʽ
#include "Word2010/CParagraph.h"	// ��������
#include "Word2010/CnlineShape.h"	// Inlineͼ�ζ��󼯺�
#include "Word2010/CnlineShapes.h"	// ����Inlineͼ�����
#include "Word2010/CRow.h"	// ������
#include "Word2010/CRows.h"	// �м���
#include "Word2010/CFields.h"
#include "Word2010/CPane.h"	// ҳü������
#include "Word2010/CWindow0.h"// ҳü������
#include "Word2010/CView0.h"// ҳü������
#include "Word2010/CPageSetup.h"	// ҳ������


//�����ö�������Ǵ�[Lib/Office2010/msword.tlh]��ȡ������,�������Խ����е�enum��ȡ����������Ҫ#import�κ�MSWORD.OLB
enum WdSaveFormat
{
	wdFormatDocument = 0,
	wdFormatDocument97 = 0,
	wdFormatTemplate = 1,
	wdFormatTemplate97 = 1,
	wdFormatText = 2,
	wdFormatTextLineBreaks = 3,
	wdFormatDOSText = 4,
	wdFormatDOSTextLineBreaks = 5,
	wdFormatRTF = 6,
	wdFormatUnicodeText = 7,
	wdFormatEncodedText = 7,
	wdFormatHTML = 8,
	wdFormatWebArchive = 9,
	wdFormatFilteredHTML = 10,
	wdFormatXML = 11,
	wdFormatXMLDocument = 12,
	wdFormatXMLDocumentMacroEnabled = 13,
	wdFormatXMLTemplate = 14,
	wdFormatXMLTemplateMacroEnabled = 15,
	wdFormatDocumentDefault = 16,
	wdFormatPDF = 17,
	wdFormatXPS = 18,
	wdFormatFlatXML = 19,
	wdFormatFlatXMLMacroEnabled = 20,
	wdFormatFlatXMLTemplate = 21,
	wdFormatFlatXMLTemplateMacroEnabled = 22,
	wdFormatOpenDocumentText = 23
};

enum WdMovementType
{
	wdMove = 0,
	wdExtend = 1
};

enum WdExportFormat
{
	wdExportFormatPDF = 17,
	wdExportFormatXPS = 18
};

enum WdExportOptimizeFor
{
	wdExportOptimizeForPrint = 0,
	wdExportOptimizeForOnScreen = 1
};

enum WdExportCreateBookmarks
{
	wdExportCreateNoBookmarks = 0,
	wdExportCreateHeadingBookmarks = 1,
	wdExportCreateWordBookmarks = 2
};

enum WdExportItem
{
	wdExportDocumentContent = 0,
	wdExportDocumentWithMarkup = 7
};

enum WdExportRange
{
	wdExportAllDocument = 0,
	wdExportSelection = 1,
	wdExportCurrentPage = 2,
	wdExportFromTo = 3
};
