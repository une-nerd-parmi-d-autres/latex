#pragma once

#include <memory>
#include <podofo/podofo.h>
#include <string>
#include <vector>
#include "invoice/Invoice.hpp"
#include "invoice/Layout.hpp"

namespace invoice
{

using PoDoFo::PdfFont;
using PoDoFo::PdfMemDocument;
using PoDoFo::PdfPage;
using PoDoFo::PdfPageSize;
using PoDoFo::PdfPainter;
using PoDoFo::PdfStrokeStyle;
using PoDoFo::Rect;

struct TextLine
{
    std::string text;
    double fontSize;
};

struct PageDefinition
{
    PdfPageSize size;
    PdfStrokeStyle strokeStyle;
    double lineWidth;
};

struct PageContext
{
    PdfPage* page;
    PdfPainter* painter;
    PdfFont* font;
};

struct InvoiceTableLayout
{
    double headerHeight;
    double rowHeight;
    double quantityWidth;
    double unitPriceWidth;
    double totalsWidth;
    double padding;
};

class InvoicePdf
{
public:
    void generate(const Invoice& invoice, const std::string& filename) const;

private:
    std::vector<PageContext> createPageContexts(PdfMemDocument& document, const std::vector<PageDefinition>& definitions, std::vector<std::unique_ptr<PdfPainter>>& painters) const;
    void drawPage(const Invoice& invoice, const PageContext& context, const Layout& layout) const;
    void drawCompanyBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    void drawInvoiceBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    void drawCustomerBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
    void drawTextBlock(PdfPainter& painter, PdfFont& font, const BlockGeometry& geometry, const std::vector<TextLine>& lines) const;
    void drawParagraph(PdfPainter& painter, PdfFont& font, const std::string& text, double x, double y, double maxWidth, double lineHeight) const;
    double drawInvoiceTable(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const;
};

}