#pragma once

struct PaperSize
{
	TCHAR cPaperName[40];	//纸型名称，//_T("Letter")
	int nPaperSize;			//纸型大小，// #define DMPAPER_LETTER
	int nPhysicalWidthMMT;	//纸型物理宽度，十分之一毫米（0.1mm）
	int nPhysicalHeightMMT;	//纸型物理高度，十分之一毫米（0.1mm）
	int nPhysicalArea;		//物理面积,长度和宽度的乘积，nPhysicalWidthMMT*nPhysicalHeightMMT
	int nPhysicalWidth;		//1200DPI为标准的纸型物理宽度，像素
	int nPhysicalHeight;	//1200DPI为标准的纸型物理高度，像素
	int nPrintWidth;		//1200DPI为标准的纸型打印宽度，像素
	int nPrintHeight;		//1200DPI为标准的纸型打印高度，像素
	int nOriginX;			//1200DPI为标准的纸型打印起始位置的X坐标，像素
	int nOriginY;			//1200DPI为标准的纸型打印起始位置的Y坐标，像素
};

#define Paper_Size_Amend_Value	1	//修正实际大小的数值

static PaperSize pagerSizetab[] = 
{
	{_T("First UnKnow"), -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{_T("Letter"), DMPAPER_LETTER, 2159, 2794, 2159*2794, 10200, 13200, 9812, 12812, 194, 194},	/* Letter 8 1/2 x 11 in               */
	{_T("Letter Small"), DMPAPER_LETTERSMALL, 2159, 2794, 2159*2794, 10200, 13200, 9812, 12812, 194, 194},	/* Letter Small 8 1/2 x 11 in         */
	{_T("Tabloid"), DMPAPER_TABLOID, 2794, 4318, 2794*4318, 13200, 20400, 12812, 20012, 0, 0},		/* Tabloid 11 x 17 in                 */
	{_T("Ledger"), DMPAPER_LEDGER, 4318, 2794, 4318*2794, 20400, 13200, 20400, 13200, 194, 194},	/* Ledger 17 x 11 in                  */
	{_T("Legal"), DMPAPER_LEGAL, 2159, 3556, 2159*3556, 10200, 16800, 9812, 16412, 194, 194},	/* Legal 8 1/2 x 14 in                */
	{_T("Statement"), DMPAPER_STATEMENT, 1397, 2159, 1397*2159, 6600, 10200, 6212, 9812, 194, 194},	/* Statement 5 1/2 x 8 1/2 in         */
	{_T("Executive"), DMPAPER_EXECUTIVE, 1841, 2667, 1841*2667, 8703, 12600, 8315, 12212, 194, 194},	/* Executive 7 1/4 x 10 1/2 in        */
	{_T("A3"), DMPAPER_A3, 2970, 4200, 2970*4200, 14032, 19843, 13644, 19455, 194, 194},	/* A3 297 x 420 mm                    */
	{_T("A4"), DMPAPER_A4, 2100, 2970, 2100*2970, 9922, 14032, 9534, 13644, 194, 194},	/* A4 210 x 297 mm                    */
	{_T("A4 Small"), DMPAPER_A4SMALL, 2100, 2970, 2100*2970, 9922, 14032, 9534, 13644, 194, 194},	/* A4 Small 210 x 297 mm              */
	{_T("A5"), DMPAPER_A5, 1480, 2100, 1480*2100, 6993, 9922, 6605, 9534, 194, 194},		/* A5 148 x 210 mm                    */
#ifndef Paper_Size_Amend_Value
	2500*3540,	/* B4 (JIS) 250 x 354                 */
#else
	{_T("B4 (JIS)"), DMPAPER_B4, 2570, 3640, 2570*3640, 12142, 17197 ,11754, 16809, 194, 194},	/* B4 (JIS) 257 x 364                 */
#endif
	{_T("B5 (JIS)"), DMPAPER_B5, 1820, 2570, 1820*2570, 8599, 12142, 8211, 11754, 194, 194},	/* B5 (JIS) 182 x 257 mm              */
	{_T("Folio"), DMPAPER_FOLIO, 2159, 3302, 2159*3302, 10200, 15600, 9812, 15212, 194, 194},	/* Folio 8 1/2 x 13 in                */
	{_T("Quarto"), DMPAPER_QUARTO, 2150, 2750, 2150*2750, 10157, 12992, 10157, 12992, 0, 0},		/* Quarto 215 x 275 mm                */
	{_T("10x14"), DMPAPER_10X14, 2540, 3556, 2540*3556, 12000, 16800, 12000, 16800, 0, 0},		/* 10x14 in                           */
	{_T("11x17"), DMPAPER_11X17, 2794, 4318, 2794*4318, 13200, 20400, 13200, 20400, 0, 0},		/* 11x17 in                           */
	{_T("Note"), DMPAPER_NOTE, 2159, 2794, 2159*2794, 10200, 13200, 10200, 13200, 0, 0},		/* Note 8 1/2 x 11 in                 */
	{_T("Envelope #9"), DMPAPER_ENV_9, 984, 2254, 984*2254, 4650, 10650, 4650, 10650, 0, 0},		/* Envelope #9 3 7/8 x 8 7/8          */
	{_T("Envelope #10"), DMPAPER_ENV_10, 1084, 2413, 1084*2413, 4952, 11400, 4564, 11012, 194, 194},   /* Envelope #10 4 1/8 x 9 1/2         */
	{_T("Envelope #11"), DMPAPER_ENV_11, 1143, 2635, 1143*2635, 5400, 12450, 5400, 12450, 0, 0},		/* Envelope #11 4 1/2 x 10 3/8        */
	{_T("Envelope #12"), DMPAPER_ENV_12, 1206, 2794, 1206*2794, 5700, 13200, 5700, 13200, 0, 0},		/* Envelope #12 4 \276 x 11           */
	{_T("Envelope #14"), DMPAPER_ENV_14, 1270, 2921, 1270*2921, 6000, 13800, 6000, 13800, 0, 0},		/* Envelope #14 5 x 11 1/2            */
	{_T("C size sheet"), DMPAPER_CSHEET, 4318, 5588, 4318*5588, 20400, 26400, 20400, 26400, 0, 0},		/* C size sheet  17 x 22 in           */
	{_T("D size sheet"), DMPAPER_DSHEET, 5588, 8636, 5588*8636, 26400, 40800, 26400, 40800, 0, 0},		/* D size sheet   22 x 34 in          */
	{_T("E size sheet"), DMPAPER_ESHEET, 8636, 11176, 8636*11176, 40800, 52800, 40800, 52800, 0, 0},		/* E size sheet    34 x 44 in         */
	{_T("Envelope DL"), DMPAPER_ENV_DL, 1100, 2200, 1100*2200, 5197, 10394, 4809, 10006, 194, 194},	/* Envelope DL 110 x 220mm            */
	{_T("Envelope C5"), DMPAPER_ENV_C5, 1620, 2290, 1620*2290, 7654, 10819, 7266, 10431, 194, 194},	/* Envelope C5 162 x 229 mm           */
	{_T("Envelope C3"), DMPAPER_ENV_C3, 3240, 4580, 3240*4580, 15307, 21637, 15307, 21637, 0, 0},		 /* Envelope C3  324 x 458 mm          */
	{_T("Envelope C4"), DMPAPER_ENV_C4, 2290, 3240, 2290*3240, 10819, 15308, 10431, 14920, 194, 194},  /* Envelope C4  229 x 324 mm          */
	{_T("Envelope C6"), DMPAPER_ENV_C6, 1140, 1620, 1140*1620, 5386, 7654, 4998, 7266, 194, 194},		/* Envelope C6  114 x 162 mm          */
	{_T("Envelope C65"), DMPAPER_ENV_C65, 1140, 2290, 1140*2290, 5385, 10818, 5385, 10818, 0, 0},		/* Envelope C65 114 x 229 mm          */
	{_T("Envelope B4"), DMPAPER_ENV_B4, 2500, 3530, 2500*3530, 11811, 16677, 11811, 16677, 0, 0},		/* Envelope B4  250 x 353 mm          */
	{_T("Envelope B5"), DMPAPER_ENV_B5, 1760, 2500, 1760*2500, 8314, 11811, 8314, 11811, 0, 0},		/* Envelope B5  176 x 250 mm          */
	{_T("Envelope B6"), DMPAPER_ENV_B6, 1760, 1250, 1760*1250, 8314, 5905, 8314, 5905, 0, 0},			/* Envelope B6  176 x 125 mm          */
	{_T("Envelope"), DMPAPER_ENV_ITALY, 1100, 2300, 1100*2300, 5196, 10866, 5196, 10866, 0, 0},		/* Envelope 110 x 230 mm              */
	{_T("Envelope Monarch"), DMPAPER_ENV_MONARCH, 984, 1905, 984*1905, 4649, 9000, 4261, 8612, 194, 194},		/* Envelope Monarch 3.875 x 7.5 in    */
	{_T("Envelope Personal"), DMPAPER_ENV_PERSONAL, 921, 1651, 921*1651, 4350, 7800, 4350, 7800, 0, 0},			/* 6 3/4 Envelope 3 5/8 x 6 1/2 in    */
	{_T("US Std Fanfold"), DMPAPER_FANFOLD_US, 3778, 2794, 3778*2794, 17850, 13200, 17850, 13200, 0, 0},  /* US Std Fanfold 14 7/8 x 11 in      */
	{_T("German Std Fanfold"), DMPAPER_FANFOLD_STD_GERMAN, 2159, 3048, 2159*3048, 10200, 14400, 10200, 14400, 0, 0},  /* German Std Fanfold 8 1/2 x 12 in   */
	{_T("German Legal Fanfold"), DMPAPER_FANFOLD_LGL_GERMAN, 2159, 3302, 2159*3302, 10200, 15600, 10200, 15600, 0, 0},  /* German Legal Fanfold 8 1/2 x 13 in */
	{_T("B4 (ISO)"), DMPAPER_ISO_B4, 2500, 3530, 2500*3530, 11811, 16677, 11811, 16677, 0, 0},  /* B4 (ISO) 250 x 353 mm              */
	{_T("Japanese Postcard"), DMPAPER_JAPANESE_POSTCARD, 1000, 1480, 1000*1480, 4725, 6993, 4337, 6605, 194, 194},  /* Japanese Postcard 100 x 148 mm     */
	{_T("9 x 11"), DMPAPER_9X11, 2286, 2794, 2286*2794, 10800, 13200, 10412, 12812, 194, 194},  /* 9 x 11 in                          */
	{_T("10 x 11"), DMPAPER_10X11, 2540, 2794, 2540*2794, 12000, 13200, 11612, 12812, 194, 194},  /* 10 x 11 in                         */
	{_T("15 x 11"), DMPAPER_15X11, 3810, 2794, 3810*2794, 18000, 13200, 18000, 13200, 0, 0},  /* 15 x 11 in                         */
	{_T("Envelope Invite"), DMPAPER_ENV_INVITE, 2200, 2200, 2200*2200, 10393, 10393, 10393, 10393, 0, 0},  /* Envelope Invite 220 x 220 mm       */
	{_T("RESERVED_48"), DMPAPER_RESERVED_48, -1, -1, -1, -1, -1, -1, -1, -1, -1},  /* RESERVED--DO NOT USE               */
	{_T("RESERVED_49"), DMPAPER_RESERVED_49, -1, -1, -1, -1, -1, -1, -1, -1, -1},  /* RESERVED--DO NOT USE               */
	{_T("Letter Extra"), DMPAPER_LETTER_EXTRA, 2413, 3048, 2413*3048, 11400, 14400, 11400, 14400, 0, 0},  /* Letter Extra 9 \275 x 12 in        */
	{_T("Legal Extra"), DMPAPER_LEGAL_EXTRA, 2413, 3810, 2413*3810, 11400, 18000, 11400, 18000, 0, 0},  /* Legal Extra 9 \275 x 15 in         */
	{_T("Tabloid Extra"), DMPAPER_TABLOID_EXTRA, 2970, 4572, 2970*4572, 14400, 21600, 14400, 21600, 0, 0},  /* Tabloid Extra 11.69 x 18 in        */
	{_T("A4 Extra"), DMPAPER_A4_EXTRA, 2354, 3223, 2354*3223, 11124, 15228, 11124, 15228, 0, 0},  /* A4 Extra 9.27 x 12.69 in           */
	{_T("Letter Transverse"), DMPAPER_LETTER_TRANSVERSE, 2159, 2794, 2159*2794, 10200, 13200, 10200, 13200, 0, 0},  /* Letter Transverse 8 \275 x 11 in   */
	{_T("A4 Transverse"), DMPAPER_A4_TRANSVERSE, 2100, 2970, 2100*2970, 9921, 14031, 9921, 14031, 0, 0},  /* A4 Transverse 210 x 297 mm         */
	{_T("Letter Extra Transverse"), DMPAPER_LETTER_EXTRA_TRANSVERSE, 2413, 3048, 2413*3048, 11400, 14400, 11400, 14400, 0, 0},  /* Letter Extra Transverse 9\275 x 12 in */
	{_T("Super A"), DMPAPER_A_PLUS, 2270, 3560, 2270*3560, 10724, 16818, 10724, 16818, 0, 0},  /* SuperA/SuperA/A4 227 x 356 mm      */
	{_T("Super B"), DMPAPER_B_PLUS, 3050, 4870, 3050*4870, 14409, 23007, 14409, 23007, 0, 0},  /* SuperB/SuperB/A3 305 x 487 mm      */
	{_T("Letter Plus"), DMPAPER_LETTER_PLUS, 2159, 3223, 2159*3223, 10200, 15228, 10200, 15228, 0, 0},  /* Letter Plus 8.5 x 12.69 in         */
	{_T("A4 Plus"), DMPAPER_A4_PLUS, 2100, 3300, 2100*3300, 9921, 15590, 9921, 15590, 0, 0},  /* A4 Plus 210 x 330 mm               */
	{_T("A5 Transverse"), DMPAPER_A5_TRANSVERSE, 1480, 2100, 1480*2100, 6992, 9921, 6992, 9921, 0, 0},  /* A5 Transverse 148 x 210 mm         */
	{_T("B5 (JIS) Transverse"), DMPAPER_B5_TRANSVERSE, 1820, 2570, 1820*2570, 8598, 12141, 8598, 12141, 0, 0},  /* B5 (JIS) Transverse 182 x 257 mm   */
	{_T("A3 Extra"), DMPAPER_A3_EXTRA, 3220, 4450, 3220*4450, 15212, 21023, 15212, 21023, 0, 0},  /* A3 Extra 322 x 445 mm              */
	{_T("A5 Extra"), DMPAPER_A5_EXTRA, 1740, 2350, 1740*2350, 8220, 11102, 8220, 11102, 0, 0},  /* A5 Extra 174 x 235 mm              */
	{_T("B5 (ISO) Extra"), DMPAPER_B5_EXTRA, 2010, 2760, 2010*2760, 9496, 13039, 9496, 13039, 0, 0},  /* B5 (ISO) Extra 201 x 276 mm        */
	{_T("A2"), DMPAPER_A2, 4200, 5940, 4200*5940, 19843, 28063, 19455, 27675, 194, 194},  /* A2 420 x 594 mm                    */
	{_T("A3 Transverse"), DMPAPER_A3_TRANSVERSE, 2970, 4200, 2970*4200, 14031, 19842, 14031, 19842, 0, 0},  /* A3 Transverse 297 x 420 mm         */
	{_T("A3 Extra Transverse"), DMPAPER_A3_EXTRA_TRANSVERSE, 3220, 4450, 3220*4450, 15212, 21023, 15212, 21023, 0, 0},  /* A3 Extra Transverse 322 x 445 mm   */
	{_T("Japanese Double Postcard"), DMPAPER_DBL_JAPANESE_POSTCARD, 2000, 1480, 2000*1480, 9448, 6992, 9448, 6992, 0, 0},  /* Japanese Double Postcard 200 x 148 mm */
	{_T("A6"), DMPAPER_A6, 1050, 1480, 1050*1480, 4961, 6993, 4573, 6605, 194, 194},  /* A6 105 x 148 mm                 */
	{_T("Japanese Envelope Kaku #2"), DMPAPER_JENV_KAKU2, 2400, 3320, 2400*3320, 11339, 15686, 10951, 15298, 194, 194},  /* Japanese Envelope Kaku #2       */
	{_T("Japanese Envelope Kaku #3"), DMPAPER_JENV_KAKU3, 2160, 2770, 2160*2770, 10204, 13086, 10204, 13086, 0, 0},  /* Japanese Envelope Kaku #3       */
	{_T("Japanese Envelope Chou #3"), DMPAPER_JENV_CHOU3, 1200, 2350, 1200*2350, 5670, 11103, 5282, 10715, 194, 194},  /* Japanese Envelope Chou #3       */
	{_T("Japanese Envelope Chou #4"), DMPAPER_JENV_CHOU4, 900, 2050, 900*2050, 4252, 9686, 3864, 9298, 194, 194},   /* Japanese Envelope Chou #4       */
	{_T("Letter Rotated"), DMPAPER_LETTER_ROTATED, 2794, 2159, 2794*2159, 13200, 10200, 13200, 10200, 0, 0},  /* Letter Rotated 11 x 8 1/2 11 in */
	{_T("A3 Rotated"), DMPAPER_A3_ROTATED, 4200, 2970, 4200*2970, 19842, 14031, 19842, 14031, 0, 0},  /* A3 Rotated 420 x 297 mm         */
	{_T("A4 Rotated"), DMPAPER_A4_ROTATED, 2970, 2100, 2970*2100, 14031, 9921, 14031, 9921, 0, 0},  /* A4 Rotated 297 x 210 mm         */
	{_T("A5 Rotated"), DMPAPER_A5_ROTATED, 2100, 1480, 2100*1480, 9921, 6992, 9921, 6992, 0, 0},  /* A5 Rotated 210 x 148 mm         */
	{_T("B4 (JIS) Rotated"), DMPAPER_B4_JIS_ROTATED, 3640, 2570, 3640*2570, 17196, 12141, 17196, 12141, 0, 0},  /* B4 (JIS) Rotated 364 x 257 mm   */
	{_T("B5 (JIS) Rotated"), DMPAPER_B5_JIS_ROTATED, 2570, 1820, 2570*1820, 12141, 8598, 12141, 8598, 0, 0},  /* B5 (JIS) Rotated 257 x 182 mm   */
	{_T("Japanese Postcard Rotated"), DMPAPER_JAPANESE_POSTCARD_ROTATED, 1480, 1000, 1480*1000, 6992, 4724, 6992, 4724, 0, 0},  /* Japanese Postcard Rotated 148 x 100 mm */
	{_T("Double Japanese Postcard Rotated"), DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED, 1480, 2000, 1480*2000, 6993, 9449, 6605, 9061, 194, 194},  /* Double Japanese Postcard Rotated 148 x 200 mm */
	{_T("A6 Rotated"), DMPAPER_A6_ROTATED, 1480, 1050, 1480*1050, 6992, 4960, 6992, 4960, 0, 0},  /* A6 Rotated 148 x 105 mm         */
	{_T("Japanese Envelope Kaku #2 Rotated"), DMPAPER_JENV_KAKU2_ROTATED, 3320, 2400, 3320*2400, 15685, 11338, 15685, 11338, 0, 0},  /* Japanese Envelope Kaku #2 Rotated */
	{_T("Japanese Envelope Kaku #3 Rotated"), DMPAPER_JENV_KAKU3_ROTATED, 2770, 2160, 2770*2160, 13086, 10204, 13086, 10204, 0, 0},  /* Japanese Envelope Kaku #3 Rotated */
	{_T("Japanese Envelope Chou #3 Rotated"), DMPAPER_JENV_CHOU3_ROTATED, 2350, 1200, 2350*1200, 11102, 5669, 11102, 5669, 0, 0},  /* Japanese Envelope Chou #3 Rotated */
	{_T("Japanese Envelope Chou #4 Rotated"), DMPAPER_JENV_CHOU4_ROTATED, 2050, 900, 2050*900, 9685, 4251, 9685, 4251, 0, 0},   /* Japanese Envelope Chou #4 Rotated */
	{_T("B6 (JIS)"), DMPAPER_B6_JIS, 1280, 1820, 1280*1820, 6048, 8599, 5660, 8211, 194, 194},  /* B6 (JIS) 128 x 182 mm           */
	{_T("B6 (JIS) Rotated"), DMPAPER_B6_JIS_ROTATED, 1820, 1280, 1820*1280, 8598, 6047, 8598, 6047, 0, 0},  /* B6 (JIS) Rotated 182 x 128 mm   */
	{_T("12 x 11"), DMPAPER_12X11, 3049, 2795, 3049*2795, 14406, 13205, 14406, 13205, 0, 0},  /* 12 x 11 in                      */
	{_T("Japanese Envelope You #4"), DMPAPER_JENV_YOU4, 1050, 2350, 1050*2350, 4960, 11102, 4960, 11102, 0, 0},  /* Japanese Envelope You #4        */
	{_T("Japanese Envelope You #4 Rotated"), DMPAPER_JENV_YOU4_ROTATED, 2350, 1050, 2350*1050, 11102, 4960, 11102, 4960, 0, 0},  /* Japanese Envelope You #4 Rotated*/
#ifndef Paper_Size_Amend_Value
	1460*2150,  /* PRC 16K 146 x 215 mm            */
	970*1510,  /* PRC 32K 97 x 151 mm             */
	970*1510,  /* PRC 32K(Big) 97 x 151 mm        */
#else
	{_T("PRC 16K"), DMPAPER_P16K, 1840, 2600, 1840*2600, 8881, 12283, 8881, 12283, 0, 0},  /* PRC 16K 184 x 260 mm            */
	{_T("PRC 32K"), DMPAPER_P32K, 1300, 1840, 1300*1840, 6141, 8692, 6141, 8692, 0, 0},  /* PRC 32K 130 x 184 mm            */
	{_T("PRC 32K(Big)"), DMPAPER_P32KBIG, 1400, 2030, 1400*2030, 6614, 9590, 6614, 9590, 0, 0},  /* PRC 32K(Big) 140 x 203 mm       */
#endif
	{_T("PRC Envelope #1"), DMPAPER_PENV_1, 1020, 1650, 1020*1650, 4818, 7795, 4818, 7795, 0, 0},  /* PRC Envelope #1 102 x 165 mm    */
	{_T("PRC Envelope #2"), DMPAPER_PENV_2, 1020, 1760, 1020*1760, 4818, 8314, 4818, 8314, 0, 0},  /* PRC Envelope #2 102 x 176 mm    */
	{_T("PRC Envelope #3"), DMPAPER_PENV_3, 1250, 1760, 1250*1760, 5905, 8314, 5905, 8314, 0, 0},  /* PRC Envelope #3 125 x 176 mm    */
	{_T("PRC Envelope #4"), DMPAPER_PENV_4, 1100, 2080, 1100*2080, 5196, 9826, 5196, 9826, 0, 0},  /* PRC Envelope #4 110 x 208 mm    */
	{_T("PRC Envelope #5"), DMPAPER_PENV_5, 1100, 2200, 1100*2200, 5196, 10393, 5196, 10393, 0, 0},  /* PRC Envelope #5 110 x 220 mm    */
	{_T("PRC Envelope #6"), DMPAPER_PENV_6, 1200, 2300, 1200*2300, 5669, 10866, 5669, 10866, 0, 0},  /* PRC Envelope #6 120 x 230 mm    */
	{_T("PRC Envelope #7"), DMPAPER_PENV_7, 1600, 2300, 1600*2300, 7559, 10866, 7559, 10866, 0, 0},  /* PRC Envelope #7 160 x 230 mm    */
	{_T("PRC Envelope #8"), DMPAPER_PENV_8, 1200, 3090, 1200*3090, 5669, 14598, 5669, 14598, 0, 0},  /* PRC Envelope #8 120 x 309 mm    */
	{_T("PRC Envelope #9"), DMPAPER_PENV_9, 2290, 3240, 2290*3240, 10818, 15307, 10818, 15307, 0, 0},  /* PRC Envelope #9 229 x 324 mm    */
	{_T("PRC Envelope #10"), DMPAPER_PENV_10, 3240, 4580, 3240*4580, 15307, 21637, 15307, 21637, 0, 0},  /* PRC Envelope #10 324 x 458 mm   */
#ifndef Paper_Size_Amend_Value
	2150*1460, /* PRC 16K Rotated 215 x 146 mm           */
	1510*970, /* PRC 32K Rotated 151 x 97 mm             */
	1510*970, /* PRC 32K(Big) Rotated  151 x 97 mm       */
#else
	{_T("PRC 16K Rotated"), DMPAPER_P16K_ROTATED, 2600, 1840, 2600*1840, 12283, 8881, 12283, 8881, 0, 0},  /* PRC 16K Rotated 260 x 184 mm	*/
	{_T("PRC 32K Rotated"), DMPAPER_P32K_ROTATED, 1840, 1300, 1840*1300, 8692, 6141, 8692, 6141, 0, 0},  /* PRC 32K Rotated 184 x 130 mm */
	{_T("PRC 32K(Big) Rotated"), DMPAPER_P32KBIG_ROTATED, 2030, 1400, 2030*1400, 9590, 6614, 9590, 6614, 0, 0},  /* PRC 32K(Big) Rotated 203 x 140 mm	*/
#endif
	{_T("PRC Envelope #1 Rotated"), DMPAPER_PENV_1_ROTATED, 1650, 1020, 1650*1020, 7795, 4818, 7795, 4818, 0, 0},  /* PRC Envelope #1 Rotated 165 x 102 mm */
	{_T("PRC Envelope #2 Rotated"), DMPAPER_PENV_2_ROTATED, 1760, 1020, 1760*1020, 8314, 4818, 8314, 4818, 0, 0},  /* PRC Envelope #2 Rotated 176 x 102 mm */
	{_T("PRC Envelope #3 Rotated"), DMPAPER_PENV_3_ROTATED, 1760, 1250, 1760*1250, 8314, 5905, 8314, 5905, 0, 0},  /* PRC Envelope #3 Rotated 176 x 125 mm */
	{_T("PRC Envelope #4 Rotated"), DMPAPER_PENV_4_ROTATED, 2080, 1100, 2080*1100, 9826, 5196, 9826, 5196, 0, 0},  /* PRC Envelope #4 Rotated 208 x 110 mm */
	{_T("PRC Envelope #5 Rotated"), DMPAPER_PENV_5_ROTATED, 2200, 1100, 2200*1100, 10393, 5196, 10393, 5196, 0, 0},  /* PRC Envelope #5 Rotated 220 x 110 mm */
	{_T("PRC Envelope #6 Rotated"), DMPAPER_PENV_6_ROTATED, 2300, 1200, 2300*1200, 10866, 5669, 10866, 5669, 0, 0},  /* PRC Envelope #6 Rotated 230 x 120 mm */
	{_T("PRC Envelope #7 Rotated"), DMPAPER_PENV_7_ROTATED, 2300, 1600, 2300*1600, 10866, 7559, 10866, 7559, 0, 0},  /* PRC Envelope #7 Rotated 230 x 160 mm */
	{_T("PRC Envelope #8 Rotated"), DMPAPER_PENV_8_ROTATED, 3090, 1200, 3090*1200, 14598, 5669, 14598, 5669, 0, 0},  /* PRC Envelope #8 Rotated 309 x 120 mm */
	{_T("PRC Envelope #9 Rotated"), DMPAPER_PENV_9_ROTATED, 3240, 2290, 3240*2290, 15307, 10818, 15307, 10818, 0, 0},  /* PRC Envelope #9 Rotated 324 x 229 mm */
	{_T("PRC Envelope #10 Rotated"), DMPAPER_PENV_10_ROTATED, 4580, 3240, 4580*3240, 21637, 15307, 21637, 15307, 0, 0},  /* PRC Envelope #10 Rotated 458 x 324 mm */
	{_T("Last UnKnow"), DMPAPER_LAST, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};
