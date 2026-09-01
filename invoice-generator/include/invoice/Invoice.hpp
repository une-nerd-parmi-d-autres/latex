#pragma once

#include <string>
#include <vector>

#include "invoice/Company.hpp"
#include "invoice/Customer.hpp"
#include "invoice/InvoiceItem.hpp"

namespace invoice
{

class Invoice
{
public:
    Company company;
    Customer customer;

    std::string number;
    std::string date;

    std::vector<InvoiceItem> items;

    bool loadFromJson(const std::string& filename);
};

} // namespace invoice