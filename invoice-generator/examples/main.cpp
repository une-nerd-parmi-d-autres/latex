#include <iostream>

#include "invoice/Invoice.hpp"
#include "invoice/InvoicePdf.hpp"

int main()
{
    invoice::Invoice invoice;

    invoice.company.name = "Mon Entreprise";
    invoice.company.address = "10 rue de Paris";
    invoice.company.phoneNumber = "06 12 34 56 78";
    invoice.company.email = "contact@example.com";

    invoice.number = "2026-001";
    invoice.date = "22/08/2026";

    invoice.customer.name = "Jean Dupont";
    invoice.customer.address = "20 rue de Lyon";


    invoice.items.push_back({
        "Developpement logiciel",
        2,
        500.00
    });
    
    invoice.items.push_back({
        "Maintenance",
        1,
        150.00
    });

    invoice::InvoicePdf generator;

    std::cout << "Entreprise : " << invoice.company.name << '\n';
    std::cout << "Facture : " << invoice.number << '\n';
    std::cout << "Client : " << invoice.customer.name << '\n';
    std::cout << "Articles : " << invoice.items.size() << '\n';

    generator.generate(
        invoice,
        "invoice.pdf"
    );

    return 0;
}