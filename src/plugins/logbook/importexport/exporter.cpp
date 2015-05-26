#include "exporter.h"

using namespace Logbook::Internal;

Exporter::Exporter(QObject* parent)
    : QObject(parent)
{
}

Exporter::~Exporter()
{
}

void Exporter::exportTo(const QString& file)
{
    iExportTo(file);
}

