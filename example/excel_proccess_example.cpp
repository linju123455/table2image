#include "ITC_table2image.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
using namespace itc::table2image;

int main()
{
    // create
    rapidjson::Document document;
    FILE* fd = fopen("../test_json/test_create_excel.json", "r");
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
    tableAttrs.cellAttrSize = cells.Size();
    std::cout << "size is " << tableAttrs.cellAttrSize << std::endl;
    tableAttrs.cellAttr = new struct CellAttr[tableAttrs.cellAttrSize];

    for (rapidjson::SizeType i = 0; i < cells.Size(); i++) {
        const rapidjson::Value& cell = cells[i];
        const rapidjson::Value& xLntTextAttr = cell["xLntTextAttr"];

        // 设置行列信息
        tableAttrs.cellAttr[i].row = cell["row"].GetInt();
        tableAttrs.cellAttr[i].column = cell["column"].GetInt();
        
        // 设置xLntTextAttr
        std::string textType = xLntTextAttr["textType"].GetString();
        if (textType == "TEXT_NUMBER") {
            tableAttrs.cellAttr[i].xLntTextAttr.textType = TEXT_NUMBER;
            tableAttrs.cellAttr[i].xLntTextAttr.dText = xLntTextAttr["dText"].GetDouble();
        }
        else if (textType == "TEXT_STRING") {
            tableAttrs.cellAttr[i].xLntTextAttr.textType = TEXT_STRING;
            tableAttrs.cellAttr[i].xLntTextAttr.sText = xLntTextAttr["sText"].GetString();
        }
        else if (textType == "TEXT_BOOL") {
            tableAttrs.cellAttr[i].xLntTextAttr.textType = TEXT_BOOL;
            tableAttrs.cellAttr[i].xLntTextAttr.bText = xLntTextAttr["bText"].GetBool();
        }
        else if (textType == "TEXT_DATE") {
            tableAttrs.cellAttr[i].xLntTextAttr.textType = TEXT_DATE;
            tableAttrs.cellAttr[i].xLntTextAttr.dateYear = xLntTextAttr["dateYear"].GetInt();
            tableAttrs.cellAttr[i].xLntTextAttr.dateMonth = xLntTextAttr["dateMonth"].GetInt();
            tableAttrs.cellAttr[i].xLntTextAttr.dateDay = xLntTextAttr["dateDay"].GetInt();
        }
    }

    SaveWorkBook(tableAttrs, "output.xlsx");
    delete[] tableAttrs.cellAttr;

    TableAttrs loadTable;
    LoadWorkBook(loadTable, "../test.xlsx");
    std::cout << "cols is : " << loadTable.columns << " rows is : " << loadTable.rows << std::endl;

    SaveWorkBook(loadTable, "testSave.xlsx");
    delete[] loadTable.cellAttr;
    return 0;
}