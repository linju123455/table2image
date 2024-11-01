/*================================================================
*   Copyright (C) 2024 All rights reserved.
*   
*   @file：ITC_table2image.h
*   @author：linju
*   @email：15013144713@163.com
*   @date ：2024-09-02
*   @berief：table convert image code
*
================================================================*/

#ifndef _ITC_TABLE_CONVERT_IMAGE_H_
#define _ITC_TABLE_CONVERT_IMAGE_H_
#include <string>

namespace itc {
namespace table2image {

enum Alignment {
    LEFT_ALIGN, /*左对齐*/
    RIGHT_ALIGN, /*右对齐*/
    CENTER_ALIGN, /*水平居中*/
    TOP_ALIGN, /*上对齐*/
    BOTTOM_ALIGN, /*下对齐*/
    VERTICAL_CENTER_ALIGN /*垂直居中*/
};

// struct ColorAttr {
//     uint8_t     r;
//     uint8_t     g;
//     uint8_t     b;
// };

enum PixelFormat {
    RGBA,
    BGRA,
    ARGB,
    ABGR
};

enum TextType {
    TEXT_NUMBER, // 数字
    TEXT_STRING, // 字符串
    TEXT_BOOL, // Bool值
    TEXT_DATE, // 日期
};

struct XLNTTextAttr {
    TextType        textType; // 文本数据类型
    double          dText; // 数字值
    std::string     sText; // 字符串值
    bool            bText; // Bool值
    int             dateYear; // 日期年
    int             dateMonth; // 日期月
    int             dateDay; // 日期天
};

struct BorderAttr {
    int         borderThickness; // 边框粗细
    int32_t     borderColor;// 颜色值
};

struct FillAttr {
    uint8_t     transparency; // 透明度
    int32_t     fillColor;// 颜色值
};

struct TextAttr {
    std::string text; // 文字内容
    int         fontSize; // 文字尺寸
    std::string fontPath; // 文字库路径
    Alignment   lAlign; // 水平方向对齐方式
    Alignment   vAlign; // 垂直方向对齐方式
    int32_t     fontColor; // 颜色值
};

struct CellAttr {
    XLNTTextAttr xLntTextAttr; // 表格读写操作的文字属性
    TextAttr    textAttr; // 文本属性
    bool        enableTextAttr; // 使能文本属性
    FillAttr    fillAttr; // 填充属性
    bool        enableFillAttr; // 使能填充属性
    BorderAttr  borderAttr; // 边框属性
    bool        enableBorderAttr; // 使能边框属性

    int         colWidth; // 列宽
    int         rowHeight; // 行高
    int         row; // 行索引（从0开始）
    int         column; // 列索引（从0开始）

    int         xStart; // 框左上角的像素坐标x值（弃用）
    int         yStart; // 框左上角的像素坐标y值（弃用）

    bool        isPartOfMerged = false; // 是否为合并单元格的从格子，默认值为false
    bool        isMerged = false; // 是否为合并单元格的主格子，默认值为false
    int         mergedColWidth; // 合并单元格后的列宽，仅当isMerged为true时生效
    int         mergedRowHeight; // 合并单元格后的行高，仅当isMerged为true时生效
};

struct TableAttrs {
    CellAttr*   cellAttr; // 单元格属性
    int         cellAttrSize; // 单元格数量

    int         rows; // 行数
    int         columns; // 列数
    int         totalWidth; // 总宽度（弃用）
    int         totalHeight; //总高度（弃用）
};

/*
绘制表格图片，背景属性不使能时，默认背景色为白色，透明度为255
结构体元素对应表格元素排列顺序：
|_0_|_1_|_2_|
|_3_|_4_|_5_|
|_6_|_7_|_8_|

传入: 
tableAttrs: 描述单元格属性的结构体

传入/传出
outputImgBufferPtr: 由调用者创建的指针，调用该函数会分配内存，但是指针的生命周期由创建者即上层管理
outputImgBufferSize：分配内存的长度

返回值:
null
*/
void DrawTable(const TableAttrs& tableAttrs, uint8_t** outputImgBufferPtr, size_t* outputImgBufferSize);

/*
保存PNG图片至本地

传入: 
imgBuffer: 分配好内存的指针，转载着需要访问图片的地址，如未空则返回-1
imgSize：内存数据的长度
totalWidth：图片总宽度
totalHeight：图片总高度
path：保存图片的路径（包括图片名称）

返回值:
返回0表示成功，返回-1表示失败
*/
int SavePNG(uint8_t* imgBuffer, size_t imgSize, int totalWidth, int totalHeight, const std::string& path, const PixelFormat srcPixelFormat);

/*
从本地获取一张PNG到内存

传入: 
path：图片的路径（包括图片名称），如果不存在，则返回-1

传入/传出
outputImgBufferPtr: 由调用者创建的指针，调用该函数会分配内存，但是指针的生命周期由创建者即上层管理
outputImgBufferSize：分配内存的长度

返回值:
返回0表示成功，返回-1表示失败
*/
int LoadPNG(const std::string& path, uint8_t** outputImgBufferPtr, size_t* outputImgBufferSize);

/*
从本地加载excel表格到内存

传入:
loadFilename：本地excel表格的路径

传出:
tableAttrs：描述单元格属性的结构体,详见XLNTTextAttr结构
*/
void LoadWorkBook(TableAttrs& tableAttrs, const std::string& loadFilename);

/*
保存一个excel表格到磁盘，支持数字，字符串，bool，日期（保存格式为ddmmyyyy）

传入:
tableAttrs：描述单元格属性的结构体,详见XLNTTextAttr结构
saveFilename：excel表格保存的路径
*/
void SaveWorkBook(const TableAttrs& tableAttrs, const std::string& saveFilename);

} // table2image
} // itc

#endif // _ITC_TABLE_CONVERT_IMAGE_H_