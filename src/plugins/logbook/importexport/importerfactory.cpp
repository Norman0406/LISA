#include "importerfactory.h"
#include "importeradif.h"

using namespace Logbook::Internal;

ImporterFactory::ImporterFactory()
{
}

ImporterFactory::~ImporterFactory()
{
}

QStringList ImporterFactory::iGetTypes() const
{
    QStringList types;
    types << ImporterAdif::getTypeStatic();
    return types;
}

Importer* ImporterFactory::iCreate(QString type, QObject* parent) const
{
    if (type == ImporterAdif::getTypeStatic())
        return new ImporterAdif(parent);

    return 0;
}
