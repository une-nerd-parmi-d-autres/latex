#include "invoice/Invoice.hpp"
#include "invoice/InvoicePdf.hpp"

int main()
{
    invoice::Invoice invoice;

    if (!invoice.loadFromJson("data/invoice.json")) {
        return 1;
    }

    invoice::InvoicePdf generator;

    generator.generate(invoice, "facture/Espace Chalosse 0126.pdf");

    return 0;
}