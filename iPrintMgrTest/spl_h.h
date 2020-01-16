/*****************************************************************************
* Terms of Use
* Copyright ? Christoph Lindemann. All rights reserved.
*
* Permission is granted to copy, distribute and/or modify this document under
* the terms of the GNU Free Documentation License, Version 1.2 or any later 
* version published by the Free Software Foundation; with no Invariant 
* Sections, no Front-Cover Texts, and no Back-Cover Texts. A copy of the 
* license is included in the section entitled "GNU Free Documentation 
* License".
*----------------------------------------------------------------------------
* History:
*  24-03-2004  Initial Document
*     Christoph Lindemann, christoph@lindemann.nu
*  
*  24-03-2004  Release to public
*     Christoph Lindemann, christoph@lindemann.nu
*  
*  10-06-2004  Added SPL_SMR_PADDING
*     Thanks to Fabian Franz
*  
*  11-06-2004  CORRECTED FAULTY INFORMATIONS
*     Christoph Lindemann, christoph@lindemann.nu
*  
*  14-06-2004  Added some text explaining the format.
*     Christoph Lindemann, christoph@lindemann.nu
*  
*  19-05-2005  Corrected typos in SMREXT definition
*     Thanks to Peter Wasser
*  
*  02-11-2005  Updated End-Of-Page records 0x0D and 0x0E
*     Thanks to Krzys
*
*  15-06-2006  Updated EMF Spool Metafile record types
*     Christoph Lindemann, christoph@lindemann.nu
*
*  16-06-2006  Added information about PRESTARTPAGE record
*     Christoph Lindemann, christoph@lindemann.nu
*
*  08-04-2008  Updated with new info found in MS Official Released Specs.
*     Christoph Lindemann, christoph@lindemann.nu
*
*****************************************************************************/
#pragma  once
#include <Windows.h>
#pragma pack(push,1)
//Spool Metafile constants
#define SPLMETA_SIGNATURE 0x00010000 //Version 1.0 

// Spool Metafile record types
// Please note remarks in the corresponding struct definitions
#if(WINVER >= 0x0400)
/* Windows NT Workstation 4.0 SP2 */
#define SRT_METAFILE          0x00000001 /*  1 Enhanced Meta File (EMF) NT4             */                                         
#define SRT_ENGINE_FONT       0x00000002 /*  2 A TrueType font definition               */
#define SRT_DEVMODE           0x00000003 /*  3 DevMode                                  */
#define SRT_TYPE1_FONT        0x00000004 /*  4 A PostScript Type 1 font definition      */
#define SRT_PRESTARTPAGE      0x00000005 /*  5 The start page for EPS                   */
#if (WINVER >= 0x0500)
/* Windows 2000 */
#define SRT_DESIGNVECTOR      0x00000006 /*  6 A font design vector (Multiple Master)   */
#define SRT_SUBSET_FONT       0x00000007 /*  7 A subset font definition                 */
#define SRT_DELTA_FONT        0x00000008 /*  8 A delta font definition                  */
#define SRT_FORM_METAFILE     0x00000009 /*  9 EMF                                      */
#define SRT_BW_METAFILE       0x0000000A /* 10 Monochrome EMF                           */ 
#define SRT_BW_FORM_METAFILE  0x0000000B /* 11 Monochrome EMF                           */
#define SRT_METAFILE_DATA     0x0000000C /* 12 Enhanced Meta File (EMF) Win2k           */ 
#define SRT_EXT_METAFILE      0x0000000D /* 13 An offset to document content            */
#define SRT_EXT_BW_METAFILE   0x0000000E /* 14 An offset to monochrome document content */
#define SRT_EXT_ENGINE_FONT   0x0000000F /* 15 An offset to a TrueType font definition  */
#define SRT_EXT_TYPE1_FONT    0x00000010 /* 16 An offset to a PS Type 1 font definition */
#define SRT_EXT_DESIGNVECTOR  0x00000011 /* 17 An offset to a font design vector        */
#define SRT_EXT_SUBSET_FONT   0x00000012 /* 18 An offset to a subset font definition    */
#define SRT_EXT_DELTA_FONT    0x00000013 /* 19 An offset to a delta font definition     */
#define SRT_EXT_PS_JOB_DATA   0x00000014 /* 20 Document-level PostScript data           */
#if (WINVER >= 0x0501)
/* Windows XP */
#define SRT_EXT_EMBED_FONT    0x00000015 /* 21 an offset to embedded font identifiers   */
#endif
#endif
#endif
/*****************************************************************************
* SPL_HEADER
*----------------------------------------------------------------------------
* SPL file header for EMFSPL files
*****************************************************************************/
typedef struct tagSPLHEADER { 
	DWORD SIGNATURE; 
	DWORD nSize;            // record size INCLUDING header 
	DWORD offDocumentName;  // offset of Job Title from start 
	DWORD offPort;          // offset of portname from start
	BYTE HeaderData[0]; 
} SPL_HEADER, *PSPL_HEADER;


/*****************************************************************************
* SMR - Base record
*----------------------------------------------------------------------------
* Base record type for the Spool Metafile.
*****************************************************************************/
typedef struct tagSMR{ 
	DWORD iType; // Spool metafile record type 
	DWORD nSize; // length of the following data 
	// NOT INCLUDING this header 
} SMR, *PSMR;

/*****************************************************************************
* SMREXT - Extended record
*----------------------------------------------------------------------------
* Contains neg. distance to start of Data
*****************************************************************************/
typedef struct tagSMREXT{ 
	SMR smr; 
	DWORD DistanceLow; 
	DWORD DistanceHigh;
} SMREXT, *PSMREXT;

/*****************************************************************************
* SMRPRESTARTPAGE - PRESTARTPAGE
*----------------------------------------------------------------------------
* Written before pagedata is written to spoolfile
* Used as a temporary "end of file" indicating following data is not
* fully spooled yet
*****************************************************************************/
typedef struct tagSMRPRESTARTPAGE{ 
	SMR smr; 
	DWORD Unknown1; 
	DWORD Unknown2; //0xFFFFFFFF
} SMRPRESTARTPAGE, *PSMRPRESTARTPAGE;

/*****************************************************************************
* SMR_PAGE - EMF/Page data
*----------------------------------------------------------------------------
* EMF/Page data
*****************************************************************************/
typedef struct tagSMRPAGE{ 
	SMR smr;   // if smr.nSize == 0, this indicates EndOfFile
	BYTE EMFData[0];
} SMR_PAGE, *PSMR_PAGE;

/*****************************************************************************
* SMR_DEVMODE - DEVMODE data
*----------------------------------------------------------------------------
* DEVMODE data
*****************************************************************************/
typedef struct tagSMRDEVMODE{ 
	SMR smr; 
	BYTE DEVMODEData[0];
} SMR_DEVMODE, *PSMR_DEVMODE;

/*****************************************************************************
* SMR_FONT - FONT data
*****************************************************************************/
typedef struct tagSMRFONT{ 
	SMR smr; 
	BYTE FONTData[0];
} SMR_FONT, *PSMR_FONT;

/*****************************************************************************
* SMR_EXTFONT - Extended Font Data
*----------------------------------------------------------------------------
* Contains neg. distance to start of
* Font Data
* Font data is typically embedded as
* GDICOMMENT in the prev EMF data
*****************************************************************************/
typedef struct tagEXTFONT{ 
	SMREXT smrext;
} SMR_EXTFONT, *PSMR_EXTFONT;

/*****************************************************************************
* SMR_EOPAGE - End of Page
*----------------------------------------------------------------------------
* Contains neg. distance to
* start of page record
*****************************************************************************/
typedef struct tagSMREOPAGE{ 
	SMREXT smrext; 
} SMR_EOPAGE, *PSMR_EOPAGE;

//typedef struct tagMETAHEADER {
//	WORD  mtType;
//	WORD  mtHeaderSize;
//	WORD  mtVersion;
//	DWORD mtSize;
//	WORD  mtNoObjects;
//	DWORD mtMaxRecord;
//	WORD  mtNoParameters;
//} METAHEADER;

#pragma pack(pop)
