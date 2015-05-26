#include "exporteradif.h"

using namespace Logbook::Internal;

ExporterAdif::ExporterAdif(QObject* parent)
    : Exporter(parent)
{
}

ExporterAdif::~ExporterAdif()
{
}

QString ExporterAdif::getTypeStatic()
{
    return QLatin1String("ADIF");
}

QString ExporterAdif::getType() const
{
    return ExporterAdif::getTypeStatic();
}

void ExporterAdif::iExportTo(const QString& file) const
{
    Q_UNUSED(file)
}

