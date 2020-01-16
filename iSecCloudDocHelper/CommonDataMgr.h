#pragma once

#define GlobalData					CQtMgrGlobalDataConfig::GetInstance()			//获取全局数据实例的宏

class CQtMgrGlobalDataConfig
{
protected:
	CQtMgrGlobalDataConfig();
public:
	static CQtMgrGlobalDataConfig& GetInstance();
	void LoadConfig();
	void CheckFileStorageList();

public:
	BOOL m_bCloudDocDealBeforeIsGz;	//云文档处理前是否压缩
	BOOL m_bCloudDocDealAfterIsGz;	//云文档处理后是否压缩
	int m_nOfficeDealFormatType;	//【office】云文档处理保存的格式，暂时只支持【pdf/tif】两种。
	int m_nOfficeDealFormatDpi;		//【office】云文档处理保存为【.tif】格式的dpi,【.pdf】不支持dpi参数。
	int m_nImageDealFormatType;		//【图片】云文档处理保存的格式，支持【jpg/png/bmp/tif/gif】。
	int m_nImageDealFormatDpi;		//【图片】云文档处理保存为【setvalue5】格式的dpi。
	BOOL bImageUsePhysicalArea;		//【图片】云文档处理使图片大小为打印设备的物理区域,不是打印区域。

};
