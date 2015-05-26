#ifndef ADIFEXPORTER_H
#define ADIFEXPORTER_H

#include "exporter.h"

namespace Logbook {
namespace Internal {

class ExporterAdif
        : public Exporter
{
public:
    ExporterAdif(QObject* parent = 0);
    ~ExporterAdif();

    static QString getTypeStatic();
    QString getType() const;

protected:
    void iExportTo(const QString&) const;
};

}
}


#endif // ADIFEXPORTER_H
