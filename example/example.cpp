#include "ITC_table2image.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
using namespace itc::table2image;

Alignment parseAlignment(const std::string& align) {
    if (align == "LEFT") return LEFT_ALIGN;
    if (align == "RIGHT") return RIGHT_ALIGN;
    if (align == "CENTER") return CENTER_ALIGN;
    if (align == "TOP") return TOP_ALIGN;
    if (align == "BOTTOM") return BOTTOM_ALIGN;
    return VERTICAL_CENTER_ALIGN;
}

int main(void) {
    // create
    rapidjson::Document document;
    // FILE* fd = fopen("../test_json/test_create_merge_by_idx.json", "r");
    FILE* fd = fopen("../test_json/test_create_by_idx.json", "r");
    if (!fd) {
        std::cerr << "fopen error.." << std::endl;
        return -1;
    }
    char readBuffer[65536];
    rapidjson::FileReadStream inputStream(fd, readBuffer, sizeof(readBuffer));
    document.ParseStream(inputStream);
    fclose(fd);

    const rapidjson::Value& cells = document["cells"];
    TableAttrs tableAttrs;
    tableAttrs.rows = document["rows"].GetInt();
    tableAttrs.columns = document["columns"].GetInt();
    // tableAttrs.totalHeight = document["totalHeight"].GetInt();
    // tableAttrs.totalWidth = document["totalWidth"].GetInt();
    int totalWidth = 0;
    int totalHeight = 0;

    tableAttrs.cellAttrSize = cells.Size();
    std::cout << "size is " << tableAttrs.cellAttrSize << std::endl;
    tableAttrs.cellAttr = new struct CellAttr[tableAttrs.cellAttrSize];
    for (int i = 0; i < tableAttrs.cellAttrSize; i++) {
        // tableAttrs.cellAttr[i].xStart = cells[i]["xStart"].GetInt();
        // tableAttrs.cellAttr[i].yStart = cells[i]["yStart"].GetInt();
        tableAttrs.cellAttr[i].row = cells[i]["row"].GetInt();
        tableAttrs.cellAttr[i].column = cells[i]["column"].GetInt();
        tableAttrs.cellAttr[i].rowHeight = cells[i]["rowHeight"].GetInt();
        tableAttrs.cellAttr[i].colWidth = cells[i]["colWidth"].GetInt();
        if (tableAttrs.cellAttr[i].column == 0) totalHeight += tableAttrs.cellAttr[i].rowHeight;
        if (tableAttrs.cellAttr[i].row == 0) totalWidth += tableAttrs.cellAttr[i].colWidth;

        if (cells[i].HasMember("isPartOfMerged") && cells[i].HasMember("isMerged")) {
            tableAttrs.cellAttr[i].isMerged = cells[i]["isMerged"].GetBool();
            tableAttrs.cellAttr[i].isPartOfMerged = cells[i]["isPartOfMerged"].GetBool();
            if (tableAttrs.cellAttr[i].isPartOfMerged)
                continue;

            if (tableAttrs.cellAttr[i].isMerged) {
                tableAttrs.cellAttr[i].mergedColWidth = cells[i]["mergedColWidth"].GetInt();
                tableAttrs.cellAttr[i].mergedRowHeight = cells[i]["mergedRowHeight"].GetInt();
            }
        }

        tableAttrs.cellAttr[i].enableFillAttr = cells[i]["enableFillAttr"].GetBool();
        if (tableAttrs.cellAttr[i].enableFillAttr) {
            tableAttrs.cellAttr[i].fillAttr.transparency = cells[i]["transparency"].GetInt();
            int b = cells[i]["fillColor"][0].GetInt();
            int g = cells[i]["fillColor"][1].GetInt();
            int r = cells[i]["fillColor"][2].GetInt();
            tableAttrs.cellAttr[i].fillAttr.fillColor = (r << 16) | (g << 8) | b;
        }

        tableAttrs.cellAttr[i].enableBorderAttr = cells[i]["enableBorderAttr"].GetBool();
        if (tableAttrs.cellAttr[i].enableBorderAttr) {
            tableAttrs.cellAttr[i].borderAttr.borderThickness = cells[i]["borderThickness"].GetInt();
            int b = cells[i]["borderColor"][0].GetInt();
            int g = cells[i]["borderColor"][1].GetInt();
            int r = cells[i]["borderColor"][2].GetInt();
            tableAttrs.cellAttr[i].borderAttr.borderColor = (r << 16) | (g << 8) | b;
        }

        tableAttrs.cellAttr[i].enableTextAttr = cells[i]["enableTextAttr"].GetBool();
        if (tableAttrs.cellAttr[i].enableTextAttr) {
            tableAttrs.cellAttr[i].textAttr.fontSize = cells[i]["fontSize"].GetInt();
            tableAttrs.cellAttr[i].textAttr.text = cells[i]["text"].GetString();
            tableAttrs.cellAttr[i].textAttr.fontPath = cells[i]["font"].GetString();
            tableAttrs.cellAttr[i].textAttr.lAlign = parseAlignment(cells[i]["hAlign"].GetString());
            tableAttrs.cellAttr[i].textAttr.vAlign = parseAlignment(cells[i]["vAlign"].GetString());
            int b = cells[i]["fontColor"][0].GetInt();
            int g = cells[i]["fontColor"][1].GetInt();
            int r = cells[i]["fontColor"][2].GetInt();
            tableAttrs.cellAttr[i].textAttr.fontColor = (r << 16) | (g << 8) | b;
        }
    }

    uint8_t *imgBuffer = nullptr;
    size_t   imgSize   = 0;
    itc::table2image::DrawTable(tableAttrs, &imgBuffer, &imgSize);
    
    std::cout << "save totalWidth is : " << totalWidth << " save totalHeight is : " << totalHeight << std::endl;
    itc::table2image::SavePNG(imgBuffer, imgSize, totalWidth, totalHeight, "./output.png", itc::table2image::PixelFormat::BGRA);
    delete[] tableAttrs.cellAttr;
    if (imgBuffer) {
        delete imgBuffer;
        imgBuffer = nullptr;
    }
    
    return 0;
}