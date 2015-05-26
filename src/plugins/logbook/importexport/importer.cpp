#include "importer.h"
#include "importerfactory.h"

using namespace Logbook::Internal;

Importer::Importer(QObject* parent)
    : QObject(parent)
{
}

Importer::~Importer()
{
}

void Importer::importFrom(const QString& file)
{
    iImportFrom(file);
}


