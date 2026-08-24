#include "invoice/InvoicePdf.hpp"
#include <podofo/podofo.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace invoice
{
namespace
{
std::string formatMoney(double value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value << " EUR";
    return stream.str();
}
}

void InvoicePdf::generate(const Invoice& invoice, const std::string& filename) const
{
    PoDoFo::PdfMemDocument document;
    auto& pages = document.GetPages();
    auto& page = pages.CreatePageAt(0, PoDoFo::PdfPageSize::A4);
    PoDoFo::PdfPainter painter;
    painter.SetCanvas(page);
    auto* font = document.GetFonts().SearchFont("Helvetica");
    painter.SetStrokeStyle(PoDoFo::PdfStrokeStyle::Solid);
    painter.GraphicsState.SetLineWidth(1.0);

    const BlockGeometry companyGeometry{50.0, 800.0, 240.0, 90.0, 10.0, 22.0, 42.0, 57.0, 72.0};
    const BlockGeometry invoiceGeometry{305.0, 800.0, 240.0, 90.0, 10.0, 25.0, 47.0, 65.0, 0.0};
    const BlockGeometry customerGeometry{50.0, 685.0, 495.0, 80.0, 10.0, 20.0, 40.0, 57.0, 0.0};

    drawCompanyBlock(painter, *font, invoice, companyGeometry);
    drawInvoiceBlock(painter, *font, invoice, invoiceGeometry);
    drawCustomerBlock(painter, *font, invoice, customerGeometry);

    double tableBottom = 0.0;
    
    const TableGeometry tableGeometry{50.0, 585.0, 495.0, 60.0, 350.0, 430.0, 340.0, 420.0, 25.0, 17.0};
    
    const double totalHT = drawItemsTable(painter, *font, invoice, tableBottom, tableGeometry);

    drawTotals(painter, *font, totalHT, tableBottom);
    drawFooter(painter, *font, invoice);

    painter.FinishDrawing();
    document.Save(filename);
}

void InvoicePdf::drawCompanyBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    painter.DrawRectangle(geometry.x, geometry.y - geometry.height, geometry.width, geometry.height);
    painter.TextState.SetFont(font, 16);
    painter.DrawText(invoice.company.name, geometry.x + geometry.padding, geometry.y - geometry.titleOffset);
    painter.TextState.SetFont(font, 10);
    painter.DrawText(invoice.company.address, geometry.x + geometry.padding, geometry.y - geometry.line1Offset);
    painter.DrawText(invoice.company.phoneNumber, geometry.x + geometry.padding, geometry.y - geometry.line2Offset);
    painter.DrawText(invoice.company.email, geometry.x + geometry.padding, geometry.y - geometry.line3Offset);
}

void InvoicePdf::drawInvoiceBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    painter.DrawRectangle(geometry.x, geometry.y - geometry.height, geometry.width, geometry.height);
    painter.TextState.SetFont(font, 18);
    painter.DrawText("FACTURE", geometry.x + geometry.padding, geometry.y - geometry.titleOffset);
    painter.TextState.SetFont(font, 10);
    const std::string number = "Numero : " + invoice.number;
    const std::string date = "Date : " + invoice.date;
    painter.DrawText(number, geometry.x + geometry.padding, geometry.y - geometry.line1Offset);
    painter.DrawText(date, geometry.x + geometry.padding, geometry.y - geometry.line2Offset);
}

void InvoicePdf::drawCustomerBlock(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    painter.DrawRectangle(geometry.x, geometry.y - geometry.height, geometry.width, geometry.height);
    painter.TextState.SetFont(font, 12);
    painter.DrawText("CLIENT", geometry.x + geometry.padding, geometry.y - geometry.titleOffset);
    painter.TextState.SetFont(font, 10);
    painter.DrawText(invoice.customer.name, geometry.x + geometry.padding, geometry.y - geometry.line1Offset);
    painter.DrawText(invoice.customer.address, geometry.x + geometry.padding, geometry.y - geometry.line2Offset);
}

double InvoicePdf::drawItemsTable(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice, double& tableBottom, const TableGeometry& geometry) const
{
    const double tableHeight = geometry.rowHeight * (invoice.items.size() + 1);
    tableBottom = geometry.y - tableHeight;

    painter.TextState.SetFont(font, 10);
    painter.DrawText("Description", geometry.descriptionX, geometry.y - geometry.textOffset);
    painter.DrawText("Quantite", geometry.quantityX, geometry.y - geometry.textOffset);
    painter.DrawText("Prix unitaire", geometry.priceX, geometry.y - geometry.textOffset);

    painter.DrawLine(geometry.x, geometry.y, geometry.x + geometry.width, geometry.y);
    painter.DrawLine(geometry.x, geometry.y - geometry.rowHeight, geometry.x + geometry.width, geometry.y - geometry.rowHeight);

    double currentY = geometry.y - geometry.rowHeight;
    double totalHT = 0.0;

    for (const auto& item : invoice.items)
    {
        totalHT += item.quantity * item.unitPrice;
        currentY -= geometry.rowHeight;
        painter.DrawText(item.description, geometry.descriptionX, currentY + geometry.textOffset);
        painter.DrawText(std::to_string(item.quantity), geometry.quantityX, currentY + geometry.textOffset);
        painter.DrawText(formatMoney(item.unitPrice), geometry.priceX, currentY + geometry.textOffset);
        painter.DrawLine(geometry.x, currentY, geometry.x + geometry.width, currentY);
    }

    painter.DrawLine(geometry.x, geometry.y, geometry.x, tableBottom);
    painter.DrawLine(geometry.quantityLineX, geometry.y, geometry.quantityLineX, tableBottom);
    painter.DrawLine(geometry.priceLineX, geometry.y, geometry.priceLineX, tableBottom);
    painter.DrawLine(geometry.x + geometry.width, geometry.y, geometry.x + geometry.width, tableBottom);

    return totalHT;
}

void InvoicePdf::drawTotals(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, double totalHT, double tableBottom) const
{
    constexpr double x = 400.0;
    const std::string totalHTText = "Total HT : " + formatMoney(totalHT);
    const std::string totalText = "TOTAL : " + formatMoney(totalHT);
    painter.TextState.SetFont(font, 10);
    painter.DrawText(totalHTText, x, tableBottom - 25);
    painter.TextState.SetFont(font, 12);
    painter.DrawText(totalText, x, tableBottom - 50);
}

void InvoicePdf::drawFooter(PoDoFo::PdfPainter& painter, PoDoFo::PdfFont& font, const Invoice& invoice) const
{
    constexpr double margin = 50.0;
    constexpr double footerY = 60.0;
    painter.DrawLine(margin, footerY + 35, 545.0, footerY + 35);
    painter.TextState.SetFont(font, 9);
    painter.DrawText("Merci pour votre confiance", 220.0, footerY + 15);
    painter.DrawText(invoice.company.name, 220.0, footerY);
    painter.DrawText(invoice.company.email, 220.0, footerY - 15);
}

}