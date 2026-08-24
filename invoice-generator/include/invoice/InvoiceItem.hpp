#pragma once

#include <string>

namespace invoice
{

class InvoiceItem
{
public:
    std::string description;
    double quantity = 0.0;
    double unitPrice = 0.0;
};

} // namespace invoice