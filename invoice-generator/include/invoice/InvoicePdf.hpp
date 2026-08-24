#pragma once
#include "invoice/Invoice.hpp"
#include <podofo/podofo.h>
#include <string>

namespace invoice
{

struct BlockGeometry
{
    double x;
    double y;
    double width;
    double height;
    double padding;
    double titleOffset;
    double line1Offset;
    double line2Offset;
    double line3Offset;
};

struct TableGeometry
{
    double x;
    double y;
    double width;
    double descriptionX;
    double quantityX;
    double priceX;
    double quantityLineX;
    double priceLineX;
    double rowHeight;
    double textOffset;
};

class InvoicePdf
{
public:
    void generate(const Invoice& invoice, const std::string& filename) const;

private:
    void drawCompanyBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    void drawInvoiceBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    void drawCustomerBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    double drawItemsTable(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, double& tableBottom, const TableGeometry& geometry) const;
    void drawTotals(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, double totalHT, double tableBottom) const;
    void drawFooter(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice) const;
};

}