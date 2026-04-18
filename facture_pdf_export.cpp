#include "facture_pdf_export.h"
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QPageSize>
#include <QPageLayout>

void FacturePdfExport::calculerTotaux(FactureData &d)
{
    d.totalHT = 0.0;
    for (const auto &l : d.lignes)
        d.totalHT += (l.total > 0) ? l.total : l.prixUnitaire * l.quantite;
    d.remise   = d.totalHT * d.remisePct / 100.0;
    double base = d.totalHT - d.remise;
    d.tva      = base * d.tvaPct / 100.0;
    d.totalTTC = base + d.tva;
}
