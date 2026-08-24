#pragma once

#include <string>

#include "invoice/Company.hpp"
#include "invoice/Customer.hpp"
#include <vector>

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
};

} // namespace invoice