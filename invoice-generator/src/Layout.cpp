#include "invoice/Layout.hpp"

#include <fstream>
#include <stdexcept>

namespace invoice
{

Layout loadLayout(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Impossible d'ouvrir le layout : " + filename);

    Layout layout;
    BlockGeometry block{};
    bool inBlock = false;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            if (inBlock)
                layout.blocks.push_back(block);

            block = {};
            inBlock = true;
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos)
            continue;

        const std::string key = line.substr(0, separator);
        const double value = std::stod(line.substr(separator + 1));

        if (key == "x")
            block.x = value;
        else if (key == "y")
            block.y = value;
        else if (key == "width")
            block.width = value;
        else if (key == "height")
            block.height = value;
        else if (key == "padding")
            block.padding = value;
        else if (key == "lineSpacing")
            block.lineSpacing = value;
    }

    if (inBlock)
        layout.blocks.push_back(block);

    return layout;
}

}