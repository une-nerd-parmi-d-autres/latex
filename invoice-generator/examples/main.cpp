#include <iostream>

#include "invoice/InvoicePdf.hpp"

int main()
{
    invoice::InvoicePdf generator;

    generator.generate("invoice.pdf");

    std::cout << "PDF generated: invoice.pdf\n";

    return 0;
}