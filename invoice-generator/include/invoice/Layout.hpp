#pragma once

#include <string>
#include <vector>

namespace invoice
{

struct BlockGeometry
{
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    double padding = 0.0;
    double lineSpacing = 0.0;
};

struct TableGeometry
{
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    double descriptionX = 0.0;
    double quantityX = 0.0;
    double priceX = 0.0;
    double quantityLineX = 0.0;
    double priceLineX = 0.0;
    double rowHeight = 0.0;
    double textOffset = 0.0;
};

struct TotalsGeometry
{
    double x = 0.0;
    double totalHTOffset = 0.0;
    double totalOffset = 0.0;
    double totalHTFontSize = 0.0;
    double totalFontSize = 0.0;
};

struct FooterGeometry
{
    double x = 0.0;
    double y = 0.0;
    double lineHeight = 0.0;
    double fontSize = 0.0;
    std::vector<std::string> texts;
    std::string paragraph;
};

struct Layout
{
    std::vector<BlockGeometry> blocks;
    TableGeometry table;
    TotalsGeometry totals;
    FooterGeometry footer;
};

Layout loadLayout(const std::string& filename);

}