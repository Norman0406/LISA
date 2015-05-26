#include "importeradif.h"

using namespace Logbook::Internal;

ImporterAdif::ImporterAdif(QObject* parent)
    : Importer(parent)
{
}

ImporterAdif::~ImporterAdif()
{
}

QString ImporterAdif::getTypeStatic()
{
    return QLatin1String("ADIF");
}

QString ImporterAdif::getType() const
{
    return ImporterAdif::getTypeStatic();
}

void ImporterAdif::iImportFrom(const QString& file) const
{
    Q_UNUSED(file)
}

