#include "exporterfactory.h"
#include "exporteradif.h"

using namespace Logbook::Internal;

ExporterFactory::ExporterFactory()
{
}

ExporterFactory::~ExporterFactory()
{
}

QStringList ExporterFactory::iGetTypes() const
{
    QStringList types;
    types << ExporterAdif::getTypeStatic();
    return types;
}

Exporter* ExporterFactory::iCreate(QString type, QObject* parent) const
{
    if (type == ExporterAdif::getTypeStatic())
        return new ExporterAdif(parent);

    return 0;
}
