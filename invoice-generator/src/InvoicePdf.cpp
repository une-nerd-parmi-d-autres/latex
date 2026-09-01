#include "invoice/InvoicePdf.hpp"
#include <iomanip>
#include <sstream>

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
    PdfMemDocument document;
    const std::vector<PageDefinition> definitions{{PdfPageSize::A4, PdfStrokeStyle::Solid, 1.0}};
    std::vector<std::unique_ptr<PdfPainter>> painters;
    const auto contexts = createPageContexts(document, definitions, painters);
    const Layout layout = loadLayout("layout/invoice.layout");

    for (const auto& context : contexts)
        drawPage(invoice, context, layout);

    document.Save(filename);
}

std::vector<PageContext> InvoicePdf::createPageContexts(PdfMemDocument& document, const std::vector<PageDefinition>& definitions, std::vector<std::unique_ptr<PdfPainter>>& painters) const
{
    PdfFont& font = document.GetFonts().GetOrCreateFont("font/NotoSans-Regular.ttf");
    std::vector<PageContext> contexts;

    painters.reserve(definitions.size());
    contexts.reserve(definitions.size());

    for (const auto& definition : definitions)
    {
        auto page = &document.GetPages().CreatePage(PdfPage::CreateStandardPageSize(definition.size));
        auto painter = std::make_unique<PdfPainter>();

        painter->SetCanvas(*page);
        painter->SetStrokeStyle(definition.strokeStyle);
        painter->GraphicsState.SetLineWidth(definition.lineWidth);

        contexts.push_back({page, painter.get(), &font});
        painters.push_back(std::move(painter));
    }

    return contexts;
}

void InvoicePdf::drawPage(const Invoice& invoice, const PageContext& context, const Layout& layout) const
{
    PdfPainter& painter = *context.painter;
    PdfFont& font = *context.font;
    const Rect pageRect = context.page->GetRect();
    const double pageWidth = pageRect.GetRight() - pageRect.GetLeft();
    const double pageHeight = pageRect.GetTop() - pageRect.GetBottom();

    painter.SetStrokeStyle(PdfStrokeStyle::Solid);
    painter.GraphicsState.SetLineWidth(1.0);
    painter.GraphicsState.SetStrokingColor(PoDoFo::PdfColor(0.8, 0.2, 0.2));
    painter.DrawRectangle(pageRect.GetLeft(), pageRect.GetBottom(), pageWidth, pageHeight);

    drawCompanyBlock(painter, font, invoice, layout.blocks[0]);
    drawInvoiceBlock(painter, font, invoice, layout.blocks[1]);
    drawInvoiceTable(painter, font, invoice, layout.blocks[2]);
    drawCustomerBlock(painter, font, invoice, layout.blocks[3]);

    painter.FinishDrawing();
}

void InvoicePdf::drawCompanyBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    const std::vector<TextLine> lines{};
    drawTextBlock(painter, font, geometry, lines);
}

void InvoicePdf::drawInvoiceBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    const std::vector<TextLine> lines{};
    drawTextBlock(painter, font, geometry, lines);
}

void InvoicePdf::drawCustomerBlock(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    const double zoneHeight = geometry.height / 3.0;
    const BlockGeometry tvaZone{geometry.x, geometry.y, geometry.width, zoneHeight, geometry.padding, geometry.lineSpacing};
    const BlockGeometry paymentZone{geometry.x, geometry.y - zoneHeight, geometry.width, zoneHeight, geometry.padding, geometry.lineSpacing};
    const BlockGeometry bankZone{geometry.x, geometry.y - zoneHeight * 2.0, geometry.width, zoneHeight, geometry.padding, geometry.lineSpacing};

    const std::vector<TextLine> paymentLines{
        {"Condition de règlement : 30 jours suivant la facture", 8.0},
        {"Modalité et condition de règlement : Virement bancaire", 8.0}
    };

    const std::vector<TextLine> bankLines{
        {"B.I.C. : BOUSFRPPXXX", 8.0},
        {"I.B.A.N. : FR76 4061 8803 5800 0402 3905 363", 8.0}
    };

    const std::vector<TextLine> legalLines{
        {"La présente facture sera payable au plus tard le : 02/08/2023. Passé ce délai, sans obligation dʼenvoi dʼune relance, conformément à lʼarticle L441-10 II du Code de Commerce, il sera appliqué une pénalité calculée à un taux annuel de 10%. Une indemnité forfaitaire pour frais de recouvrement de 40€ sera aussi exigible. Escompte pour paiement anticipé : néant. Les opérations donnant lieu à facture sont constituées exclusivement de prestations de services.", 8.0},
        {"N° Siret 923 249 908 00016", 8.0},
        {"Code APE 8552Z", 8.0}
    };

    drawTextBlock(painter, font, tvaZone, paymentLines);
    drawTextBlock(painter, font, paymentZone, bankLines);
    drawTextBlock(painter, font, bankZone, legalLines);
}

void InvoicePdf::drawTextBlock(PdfPainter& painter, PdfFont& font, const BlockGeometry& geometry, const std::vector<TextLine>& lines) const
{
    const double margin = 5.0;
    const double lineGap = 3.0;
    const double x = geometry.x + geometry.padding + margin;
    const double maxWidth = geometry.width - 2.0 * (geometry.padding + margin);
    const double topY = geometry.y - geometry.padding - margin;

    struct WrappedLine
    {
        std::string text;
        double fontSize;
    };

    std::vector<WrappedLine> wrappedLines;

    for (const auto& line : lines)
    {
        painter.TextState.SetFont(font, line.fontSize);
        const auto splitLines = painter.TextState.GetState().SplitTextAsLines(line.text, maxWidth);

        for (const auto& splitLine : splitLines)
            wrappedLines.push_back({splitLine, line.fontSize});
    }

    painter.DrawRectangle(geometry.x, geometry.y - geometry.height, geometry.width, geometry.height);

    double currentY = topY;

    for (const auto& line : wrappedLines)
    {
        painter.TextState.SetFont(font, line.fontSize);
        painter.DrawText(line.text, x, currentY);
        currentY -= line.fontSize + lineGap;
    }
}

void InvoicePdf::drawParagraph(PdfPainter& painter, PdfFont& font, const std::string& text, double x, double y, double maxWidth, double lineHeight) const
{
    painter.TextState.SetFont(font, painter.TextState.GetState().FontSize);
    const auto lines = painter.TextState.GetState().SplitTextAsLines(text, maxWidth);

    for (std::size_t i = 0; i < lines.size(); ++i)
        painter.DrawText(lines[i], x, y - static_cast<double>(i) * lineHeight);
}

double InvoicePdf::drawInvoiceTable(PdfPainter& painter, PdfFont& font, const Invoice& invoice, const BlockGeometry& geometry) const
{
    const InvoiceTableLayout table{25.0, 25.0, 80.0, 110.0, 200.0, geometry.padding};
    const double left = geometry.x;
    const double right = geometry.x + geometry.width;
    const double top = geometry.y;
    const double headerBottom = top - table.headerHeight;
    const double quantityLineX = right - table.unitPriceWidth - table.quantityWidth;
    const double priceLineX = right - table.unitPriceWidth;
    const double descriptionX = left + table.padding;
    const double quantityX = quantityLineX + table.padding;
    const double priceX = priceLineX + table.padding;
    const double itemsBottom = headerBottom - table.rowHeight * invoice.items.size();

    painter.TextState.SetFont(font, 10.0);
    painter.DrawText("Description", descriptionX, top - table.padding);
    painter.DrawText("Quantite", quantityX, top - table.padding);
    painter.DrawText("Prix unitaire", priceX, top - table.padding);

    painter.DrawLine(left, top, right, top);
    painter.DrawLine(left, headerBottom, right, headerBottom);

    double currentY = headerBottom;
    double totalHT = 0.0;

    for (const auto& item : invoice.items)
    {
        totalHT += item.quantity * item.unitPrice;
        currentY -= table.rowHeight;

        painter.DrawText(item.description, descriptionX, currentY + 8.0);
        painter.DrawText(std::to_string(item.quantity), quantityX, currentY + 8.0);
        painter.DrawText(formatMoney(item.unitPrice), priceX, currentY + 8.0);
        painter.DrawLine(left, currentY, right, currentY);
    }

    painter.DrawLine(left, top, left, itemsBottom);
    painter.DrawLine(quantityLineX, top, quantityLineX, itemsBottom);
    painter.DrawLine(priceLineX, top, priceLineX, itemsBottom);
    painter.DrawLine(right, top, right, itemsBottom);

    const double totalsTop = itemsBottom;
    const double totalsBottom = totalsTop - table.rowHeight * 3.0;
    const double totalsX = right - table.totalsWidth;

    painter.DrawLine(totalsX, totalsTop, right, totalsTop);
    painter.DrawLine(totalsX, totalsBottom, right, totalsBottom);
    painter.DrawLine(totalsX, totalsTop, totalsX, totalsBottom);
    painter.DrawLine(right, totalsTop, right, totalsBottom);
    painter.DrawLine(totalsX, totalsTop - table.rowHeight, right, totalsTop - table.rowHeight);
    painter.DrawLine(totalsX, totalsTop - table.rowHeight * 2.0, right, totalsTop - table.rowHeight * 2.0);

    painter.TextState.SetFont(font, 10.0);
    painter.DrawText("Total HT", totalsX + table.padding, totalsTop - 17.0);
    painter.DrawText(formatMoney(totalHT), totalsX + 100.0, totalsTop - 17.0);
    painter.DrawText("TVA", totalsX + table.padding, totalsTop - table.rowHeight - 17.0);
    painter.DrawText("0.00 EUR", totalsX + 100.0, totalsTop - table.rowHeight - 17.0);

    painter.TextState.SetFont(font, 11.0);
    painter.DrawText("TOTAL", totalsX + table.padding, totalsTop - table.rowHeight * 2.0 - 17.0);
    painter.DrawText(formatMoney(totalHT), totalsX + 100.0, totalsTop - table.rowHeight * 2.0 - 17.0);

    return totalHT;
}

}