#include "invoice/InvoicePdf.hpp"

#include <podofo/podofo.h>

namespace invoice
{

void InvoicePdf::generate(const std::string& filename) const
{
    PoDoFo::PdfMemDocument document;

    document.GetPages().CreatePageAt(0, PoDoFo::PdfPageSize::A4);

    document.Save(filename);
}

} // namespace invoice