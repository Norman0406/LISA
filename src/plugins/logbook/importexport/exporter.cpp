#include "exporter.h"

using namespace Logbook::Internal;

Exporter::Exporter(QObject* parent)
    : QObject(parent)
{
}

Exporter::~Exporter()
{
}

void Exporter::doExport(QModelIndexList& selection)
{
    iDoExport(selection);
}

