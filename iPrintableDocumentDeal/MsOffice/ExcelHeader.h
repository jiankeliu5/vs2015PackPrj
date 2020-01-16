#pragma once

// #include "MSOfficeHeader.h"
// 
// // Office Excel 2010 lib
// #if 0	//��������Ҫʾ�����ٴβųɹ�������ʹ�õ��������Ŀ����벻ͨ��
// #import "../Lib/Office2010/EXCEL.EXE" \
// 	no_namespace raw_interfaces_only \
// 	rename("RGB", "Excel_RGB") \
// 	rename("DialogBox", "Excel_DialogBox") \
// 	rename("CopyFile", "Excel_CopyFile") \
// 	rename("ReplaceText", "Excel_ReplaceText")
// 
// #else	//excel.tlh������#import���ɵ��ļ�
// #include "../Lib/Office2010/excel.tlh"
// #endif


//�����а�����ͷ�ļ�ע��import
//#import "C:\\Program Files (x86)\\Microsoft Office\\Office14\\EXCEL.EXE" no_namespace
//���԰���Excel2010�µ�����ͷ�ļ���Ŀǰֻ���������õ���ͷ�ļ�
#include "Excel2010/CApplication0.h"	//��ʾ������ExcelӦ�ó��򣬰���һ������������
#include "Excel2010/CWorkbooks.h"		//���������ϣ�����N����������Workbook��
#include "Excel2010/CWorkbook.h"		//������������һ��������sheets������
#include "Excel2010/CWorksheets.h"		//�������ϣ�����N��������
#include "Excel2010/CWorksheet.h"		//������Ҳ����������Excel�п�����Sheet1��Sheet2��Sheet3���������ǲ���Excel�Ļ�����λ
#include "Excel2010/CPageSetup0.h"		//ҳ�����ã���ֽ�ͣ����򣬴�ӡ���õȵ�


//�����ö�������Ǵ�[Lib/Office2010/excel.tlh]��ȡ������,�������Խ����е�enum��ȡ����������Ҫ#import�κ�EXCEL.EXE
enum XlSheetType
{
	xlChart = -4109,
	xlDialogSheet = -4116,
	xlExcel4IntlMacroSheet = 4,
	xlExcel4MacroSheet = 3,
	xlWorksheet = -4167
};

enum XlSaveAsAccessMode
{
	xlExclusive = 3,
	xlNoChange = 1,
	xlShared = 2
};

enum XlFileFormat
{
	xlAddIn = 18,
	xlCSV = 6,
	xlCSVMac = 22,
	xlCSVMSDOS = 24,
	xlCSVWindows = 23,
	xlDBF2 = 7,
	xlDBF3 = 8,
	xlDBF4 = 11,
	xlDIF = 9,
	xlExcel2 = 16,
	xlExcel2FarEast = 27,
	xlExcel3 = 29,
	xlExcel4 = 33,
	xlExcel5 = 39,
	xlExcel7 = 39,
	xlExcel9795 = 43,
	xlExcel4Workbook = 35,
	xlIntlAddIn = 26,
	xlIntlMacro = 25,
	xlWorkbookNormal = -4143,
	xlSYLK = 2,
	xlTemplate = 17,
	xlCurrentPlatformText = -4158,
	xlTextMac = 19,
	xlTextMSDOS = 21,
	xlTextPrinter = 36,
	xlTextWindows = 20,
	xlWJ2WD1 = 14,
	xlWK1 = 5,
	xlWK1ALL = 31,
	xlWK1FMT = 30,
	xlWK3 = 15,
	xlWK4 = 38,
	xlWK3FM3 = 32,
	xlWKS = 4,
	xlWorks2FarEast = 28,
	xlWQ1 = 34,
	xlWJ3 = 40,
	xlWJ3FJ3 = 41,
	xlUnicodeText = 42,
	xlHtml = 44,
	xlWebArchive = 45,
	xlXMLSpreadsheet = 46,
	xlExcel12 = 50,
	xlOpenXMLWorkbook = 51,
	xlOpenXMLWorkbookMacroEnabled = 52,
	xlOpenXMLTemplateMacroEnabled = 53,
	xlTemplate8 = 17,
	xlOpenXMLTemplate = 54,
	xlAddIn8 = 18,
	xlOpenXMLAddIn = 55,
	xlExcel8 = 56,
	xlOpenDocumentSpreadsheet = 60,
	xlWorkbookDefault = 51
};

enum XlFixedFormatType
{
	xlTypePDF = 0,
	xlTypeXPS = 1
};

enum XlFixedFormatQuality
{
	xlQualityStandard = 0,
	xlQualityMinimum = 1
};

enum XlPageOrientation
{
	xlLandscape = 2,
	xlPortrait = 1
};

enum XlPaperSize
{
	xlPaper10x14 = 16,
	xlPaper11x17 = 17,
	xlPaperA3 = 8,
	xlPaperA4 = 9,
	xlPaperA4Small = 10,
	xlPaperA5 = 11,
	xlPaperB4 = 12,
	xlPaperB5 = 13,
	xlPaperCsheet = 24,
	xlPaperDsheet = 25,
	xlPaperEnvelope10 = 20,
	xlPaperEnvelope11 = 21,
	xlPaperEnvelope12 = 22,
	xlPaperEnvelope14 = 23,
	xlPaperEnvelope9 = 19,
	xlPaperEnvelopeB4 = 33,
	xlPaperEnvelopeB5 = 34,
	xlPaperEnvelopeB6 = 35,
	xlPaperEnvelopeC3 = 29,
	xlPaperEnvelopeC4 = 30,
	xlPaperEnvelopeC5 = 28,
	xlPaperEnvelopeC6 = 31,
	xlPaperEnvelopeC65 = 32,
	xlPaperEnvelopeDL = 27,
	xlPaperEnvelopeItaly = 36,
	xlPaperEnvelopeMonarch = 37,
	xlPaperEnvelopePersonal = 38,
	xlPaperEsheet = 26,
	xlPaperExecutive = 7,
	xlPaperFanfoldLegalGerman = 41,
	xlPaperFanfoldStdGerman = 40,
	xlPaperFanfoldUS = 39,
	xlPaperFolio = 14,
	xlPaperLedger = 4,
	xlPaperLegal = 5,
	xlPaperLetter = 1,
	xlPaperLetterSmall = 2,
	xlPaperNote = 18,
	xlPaperQuarto = 15,
	xlPaperStatement = 6,
	xlPaperTabloid = 3,
	xlPaperUser = 256
};
