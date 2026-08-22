#pragma once

#include <string>

namespace invoice
{

class InvoicePdf
{
public:
    void generate(const std::string& filename) const;
};

} // namespace invoice