#include "ITC_table2image.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/freetype.hpp>
#include <xlnt/xlnt.hpp>

namespace itc {
namespace table2image {

void drawText(const cv::Mat& image, const std::string& fontPath, const std::string& text, 
              int fontSize, const cv::Scalar& fontColor,
              int xStart, int yStart, int colWidth, int rowHeight, int borderThickness,
              const Alignment& hAlign, const Alignment& vAlign) {
    cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData(fontPath, 0);
    int baseline = 0;
    // borderThickness = borderThickness / 2;
    cv::Size textSize = ft2->getTextSize(text, fontSize, -1, &baseline);
    int xText, yText;
    if (textSize.width > colWidth - (2 * borderThickness) || 
        textSize.height > rowHeight - (2 * borderThickness)) {
        cv::Rect roi(xStart + borderThickness, yStart + borderThickness, 
                        colWidth - (2 * borderThickness), rowHeight - (2 * borderThickness));
        if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width <= image.cols && roi.y + roi.height <= image.rows) {
            // std::cout << "success!" << std::endl;
        } else {
            std::cout << "ROI is out of bounds!" << std::endl;
            return;
        }

        cv::Mat roiImage = image(roi);
        switch (hAlign) {
            case LEFT_ALIGN:
                xText = 0;
                break;
            case RIGHT_ALIGN:
                xText = colWidth - textSize.width - (2 * borderThickness);
                break;
            case CENTER_ALIGN:
                xText = (colWidth - (2 * borderThickness) - textSize.width) / 2;
                break;
        }

        switch (vAlign) {
            case TOP_ALIGN:
                yText = textSize.height;
                break;
            case BOTTOM_ALIGN:
                yText = rowHeight - (2 * borderThickness) - baseline;
                break;
            case VERTICAL_CENTER_ALIGN:
                yText = (rowHeight - (2 * borderThickness) + textSize.height - baseline) / 2;
                break;
        }

        ft2->putText(roiImage, text, cv::Point(xText, yText), fontSize, fontColor, -1, cv::LINE_AA, true);
    } else {
        switch (hAlign) {
            case LEFT_ALIGN:
                xText = borderThickness;
                break;
            case RIGHT_ALIGN:
                xText = colWidth - textSize.width - borderThickness;
                break;
            case CENTER_ALIGN:
                xText = (colWidth - textSize.width) / 2;
                break;
        }

        switch (vAlign) {
            case TOP_ALIGN:
                yText = textSize.height + borderThickness;
                break;
            case BOTTOM_ALIGN:
                yText = rowHeight - borderThickness - baseline;
                break;
            case VERTICAL_CENTER_ALIGN:
                yText = (rowHeight + textSize.height - baseline) / 2;
                break;
        }
        ft2->putText(image, text, cv::Point(xStart + xText, yStart + yText), fontSize, fontColor, -1, cv::LINE_AA, true);
    }                                
}

void DrawTable(const TableAttrs& tableAttrs, uint8_t** outputImgBufferPtr, size_t* outputImgBufferSize)
{
    // 获取图像的总宽度和高度
    // int totalWidth = tableAttrs.totalWidth;
    // int totalHeight = tableAttrs.totalHeight;

    // 获取行数和列数
    int rows = tableAttrs.rows;
    int cols = tableAttrs.columns;

    // 获取单元格数量
    int cellSize = tableAttrs.cellAttrSize;

    // 获取图像的总宽度和高度
    int totalWidth = 0;
    int totalHeight = 0;
    for (int a = 0; a < cellSize; a++) {
        int rowHeight = tableAttrs.cellAttr[a].rowHeight;
        int colWidth = tableAttrs.cellAttr[a].colWidth;
        if (tableAttrs.cellAttr[a].column == 0) totalHeight += rowHeight;
        if (tableAttrs.cellAttr[a].row == 0) totalWidth += colWidth;
    }

    // 创建坐标映射数组
    std::vector<int> xCoords(cols, 0);
    std::vector<int> yCoords(rows, 0);
    
    // 计算每个格子的起始x坐标
    int currentX = 0;
    for (int c = 0; c < cols; c++) {
        xCoords[c] = currentX;
        for (int i = 0; i < cellSize; i++) {
            if (tableAttrs.cellAttr[i].column == c && tableAttrs.cellAttr[i].row == 0) {
                currentX += tableAttrs.cellAttr[i].colWidth;
                break;
            }
        }
    }

    // 计算每个格子的起始y坐标
    int currentY = 0;
    for (int r = 0; r < rows; r++) {
        yCoords[r] = currentY;
        for (int i = 0; i < cellSize; i++) {
            if (tableAttrs.cellAttr[i].row == r && tableAttrs.cellAttr[i].column == 0) {
                currentY += tableAttrs.cellAttr[i].rowHeight;
                break;
            }
        }
    }


    // 创建图像
    cv::Mat image(totalHeight, totalWidth, CV_8UC4, cv::Scalar(0, 0, 0, 0));
    printf("totalHeight is %d, totalWidth is %d\n", totalHeight, totalWidth);
    for (int i = 0; i < cellSize; i++) {
        // int xStart = tableAttrs.cellAttr[i].xStart;
        // int yStart = tableAttrs.cellAttr[i].yStart;

        // 跳过被合并的格子
        if (tableAttrs.cellAttr[i].isPartOfMerged) {
            continue;
        }
        int xStart = xCoords[tableAttrs.cellAttr[i].column];
        int yStart = yCoords[tableAttrs.cellAttr[i].row];
        int row = tableAttrs.cellAttr[i].row;
        int col = tableAttrs.cellAttr[i].column;
        int colWidth = 0;
        int rowHeight = 0;
        if (tableAttrs.cellAttr[i].isMerged) {
            colWidth = tableAttrs.cellAttr[i].mergedColWidth;
            rowHeight = tableAttrs.cellAttr[i].mergedRowHeight;
        } else {
            colWidth = tableAttrs.cellAttr[i].colWidth;
            rowHeight = tableAttrs.cellAttr[i].rowHeight;
        }

        // int colWidth = tableAttrs.cellAttr[i].colWidth;
        // int rowHeight = tableAttrs.cellAttr[i].rowHeight;

        // // 计算单元格的起始位置
        // for (int c = 0; c < col; ++c) {
        //     xStart += tableAttrs.cellAttr[c].colWidth;
        // }
        // for (int r = 0; r < row; ++r) {
        //     yStart += tableAttrs.cellAttr[r * cols].rowHeight;
        // }
        // printf("x is : %d, y is %d\n", xStart, yStart);

        // 绘制填充颜色
        if (tableAttrs.cellAttr[i].enableFillAttr) {
            int32_t b = tableAttrs.cellAttr[i].fillAttr.fillColor & 0xFF;
            int32_t g = (tableAttrs.cellAttr[i].fillAttr.fillColor & 0xFF00) >> 8;
            int32_t r = (tableAttrs.cellAttr[i].fillAttr.fillColor & 0xFF0000) >> 16;
            int transparency = tableAttrs.cellAttr[i].fillAttr.transparency;
            cv::Scalar fillColor(b, g, r, transparency);
            cv::rectangle(image, cv::Point(xStart, yStart), cv::Point(xStart + colWidth, yStart + rowHeight),
                          fillColor, cv::FILLED);
        } else {
            cv::rectangle(image, cv::Point(xStart, yStart), cv::Point(xStart + colWidth, yStart + rowHeight),
                          cv::Scalar(255, 255, 255, 255), cv::FILLED);// 默认背景颜色为白色，透明度为255
        }

        // 绘制边框
        int borderThickness = 0;
        if (tableAttrs.cellAttr[i].enableBorderAttr) {
            int32_t b = tableAttrs.cellAttr[i].borderAttr.borderColor & 0xFF;
            int32_t g = (tableAttrs.cellAttr[i].borderAttr.borderColor & 0xFF00) >> 8;
            int32_t r = (tableAttrs.cellAttr[i].borderAttr.borderColor & 0xFF0000) >> 16;
            borderThickness = tableAttrs.cellAttr[i].borderAttr.borderThickness;
            cv::Scalar borderColor(b, g, r, 255);
            cv::rectangle(image, cv::Point(xStart, yStart), cv::Point(xStart + colWidth - 1, yStart + rowHeight - 1),
                          borderColor, borderThickness);
            // printf("xStart is : %d, yStart is : %d, xend is : %d, yend : %d\n", 
            //         xStart, yStart, xStart + colWidth - 1, yStart + rowHeight - 1);
        }

        // 绘制字体
        if (tableAttrs.cellAttr[i].enableTextAttr) {
            std::string text = tableAttrs.cellAttr[i].textAttr.text;
            std::string fontPath = tableAttrs.cellAttr[i].textAttr.fontPath;
            int fontSize = tableAttrs.cellAttr[i].textAttr.fontSize;
            Alignment lAlign = tableAttrs.cellAttr[i].textAttr.lAlign;
            Alignment vAlign = tableAttrs.cellAttr[i].textAttr.vAlign;
            int32_t b = tableAttrs.cellAttr[i].textAttr.fontColor & 0xFF;
            int32_t g = (tableAttrs.cellAttr[i].textAttr.fontColor & 0xFF00) >> 8;
            int32_t r = (tableAttrs.cellAttr[i].textAttr.fontColor & 0xFF0000) >> 16;
            cv::Scalar fontColor(b, g, r, 255);
            drawText(image, fontPath, text, fontSize, fontColor, 
                     xStart, yStart, colWidth, rowHeight, borderThickness,
                     lAlign, vAlign);
        }
    }

    size_t dataLength = image.total() * image.elemSize();
    *outputImgBufferPtr = new uint8_t[dataLength];
    memcpy(*outputImgBufferPtr, image.data, dataLength);
}

int SavePNG(uint8_t* imgBuffer, size_t imgSize, int totalWidth, int totalHeight, const std::string& path, const PixelFormat srcPixelFormat)
{
    if (imgBuffer == nullptr) {
        std::cerr << "SavePNG Buffer is null!!" << std::endl;
        return -1;
    }
    cv::Mat image(totalHeight, totalWidth, CV_8UC4, imgBuffer);
    if (srcPixelFormat == PixelFormat::RGBA) {
        cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
        cv::imwrite(path, image);
        return 0;
    } else if (srcPixelFormat == PixelFormat::ARGB) {
        cv::Mat argb_image(totalHeight, totalWidth, CV_8UC4);
        // 定义通道映射：BGRA -> ARGB
        int from_to[] = { 3,0, 0,1, 1,2, 2,3 };

        cv::mixChannels(&image, 1, &argb_image, 1, from_to, 4);
        cv::imwrite(path, argb_image);
        return 0;
    } else if (srcPixelFormat == PixelFormat::ABGR) {
        cv::Mat abgr_image(totalHeight, totalWidth, CV_8UC4);
        // 定义通道映射：BGRA -> ABGR
        int from_to[] = { 3,0, 0,1, 1,2, 2,3 };

        cv::mixChannels(&image, 1, &abgr_image, 1, from_to, 4);
        cv::imwrite(path, abgr_image);
        return 0;
    } else if (srcPixelFormat == PixelFormat::BGRA) {
        cv::imwrite(path, image);
        return 0;
    }

    return -1;
}

int LoadPNG(const std::string& path, uint8_t** outputImgBufferPtr, size_t* outputImgBufferSize)
{
    cv::Mat image = cv::imread(path);
    // 检查图像是否加载成功
    if(image.empty()) {
        std::cerr << "Failed to load image." << std::endl;
        return -1;
    }

    size_t dataLength = image.total() * image.elemSize();
    *outputImgBufferPtr = new uint8_t[dataLength];
    memcpy(*outputImgBufferPtr, image.data, dataLength);
    return 0;
}

void SaveWorkBook(const TableAttrs& tableAttrs, const std::string& saveFilename)
{
    try {
        xlnt::workbook wb;
        xlnt::worksheet ws = wb.active_sheet();
        int cellAttrSize = tableAttrs.cellAttrSize;
        for (int i = 0; i < cellAttrSize; i++) {
            auto cellAttr = tableAttrs.cellAttr[i];

            int column = cellAttr.column + 1;
            int row = cellAttr.row + 1;
            auto textAttr = cellAttr.xLntTextAttr;
            switch (textAttr.textType)
            {
            case TEXT_NUMBER:
                ws.cell(column, row).value(textAttr.dText);
                break;
            case TEXT_STRING:
                ws.cell(column, row).value(textAttr.sText);
                break;
            case TEXT_BOOL:
                ws.cell(column, row).value(textAttr.bText);
                break;
            case TEXT_DATE:
                {
                    xlnt::date date(textAttr.dateYear, textAttr.dateMonth, textAttr.dateDay);
                    ws.cell(column, row).value(date);
                    ws.cell(column, row).number_format(xlnt::number_format::date_ddmmyyyy());
                }
                break;
            default:
                break;
            }
        }
        wb.save(saveFilename);
    } catch (const std::exception& e) {
        std::cerr << "保存失败，错误信息: " << e.what() << std::endl;
    }
}

void LoadWorkBook(TableAttrs& tableAttrs, const std::string& loadFilename) 
{
    xlnt::workbook wb;
    wb.load(loadFilename); // 加载 Excel 文件
    xlnt::worksheet ws = wb.active_sheet();
    int rowNumber = 0;
    int columnNumber = 0;
    int cellSize = 0;
    for (auto row : ws.rows()) {
        for (auto cell : row) {
            if (cell.has_value()) {
                cellSize++;
            }
        }
    }

    tableAttrs.cellAttrSize = cellSize;
    std::cout << "size is " << tableAttrs.cellAttrSize << std::endl;
    tableAttrs.cellAttr = new struct CellAttr[tableAttrs.cellAttrSize];
    int cell_idx = 0;
    for (auto row : ws.rows(false)) {
        for (auto cell : row) {
            std::cout << " (第" << rowNumber << "行, 第" << columnNumber << "列): ";
            if (cell.has_value()) {
                if (cell_idx > cellSize) {
                    std::cout << "cell idx : " << cell_idx << " > " << " cellSize is : " << cellSize << std::endl;
                    return;
                }
                CellAttr& cellAttr = tableAttrs.cellAttr[cell_idx++];
                cellAttr.column = columnNumber;
                cellAttr.row = rowNumber;
                // 检查是否是日期格式
                bool is_date = false;
                try {
                    auto format = cell.number_format().format_string();
                    if (format.find("yy") != std::string::npos || 
                        format.find("mm") != std::string::npos || 
                        format.find("dd") != std::string::npos) {
                        is_date = true;
                    }
                } catch (...) {
                    // 忽略格式检查错误
                }

                switch (cell.data_type()) {
                    case xlnt::cell_type::date:
                        {
                            xlnt::date date_value = cell.value<xlnt::date>();
                            cellAttr.xLntTextAttr.dateYear = date_value.year;
                            cellAttr.xLntTextAttr.dateMonth = date_value.month;
                            cellAttr.xLntTextAttr.dateDay = date_value.day;
                            cellAttr.xLntTextAttr.textType = TextType::TEXT_DATE;
                            std::cout << "日期: " << "\t";
                        }
                        break;
                    case xlnt::cell_type::number:
                        if (is_date) {
                            xlnt::date date_value = cell.value<xlnt::date>();
                            cellAttr.xLntTextAttr.dateYear = date_value.year;
                            cellAttr.xLntTextAttr.dateMonth = date_value.month;
                            cellAttr.xLntTextAttr.dateDay = date_value.day;
                            cellAttr.xLntTextAttr.textType = TextType::TEXT_DATE;
                            std::cout << "日期: " << cellAttr.xLntTextAttr.dateDay << "/" << date_value.month 
                                      << "/" << date_value.year << "\t";
                        } else {
                            cellAttr.xLntTextAttr.textType = TextType::TEXT_NUMBER;
                            cellAttr.xLntTextAttr.dText = cell.value<double>();
                            std::cout << "数字: " << cell.value<double>() << "\t";
                        }
                        break;
                    case xlnt::cell_type::inline_string:
                    case xlnt::cell_type::shared_string:
                        cellAttr.xLntTextAttr.textType = TextType::TEXT_STRING;
                        cellAttr.xLntTextAttr.sText = cell.value<std::string>();
                        std::cout << "字符串: " << cell.value<std::string>() << "\t";
                        break;
                    case xlnt::cell_type::boolean:
                        cellAttr.xLntTextAttr.textType = TextType::TEXT_BOOL;
                        cellAttr.xLntTextAttr.sText = cell.value<bool>();
                        std::cout << "布尔值: " << cell.value<bool>() << "\t";
                        break;
                    case xlnt::cell_type::empty:
                        cellAttr.xLntTextAttr.textType = TextType::TEXT_STRING;
                        cellAttr.xLntTextAttr.sText = cell.to_string();
                        std::cout << "空" << "\t";
                        break;
                    default:
                        cellAttr.xLntTextAttr.textType = TextType::TEXT_STRING;
                        cellAttr.xLntTextAttr.sText = cell.to_string();
                        std::cout << "其他类型: " << cell.to_string() << "\t";
                        break;
                }
            } else {
                std::cout << "空单元格";
            }
            columnNumber++;
        }
        tableAttrs.columns = columnNumber;
        columnNumber = 0;
        std::cout << std::endl;
        rowNumber++;
        tableAttrs.rows = rowNumber;
    }
}

} // table2image
} // itc