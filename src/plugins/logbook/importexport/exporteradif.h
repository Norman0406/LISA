#ifndef ADIFEXPORTER_H
#define ADIFEXPORTER_H

#include "exporter.h"
#include <QXmlStreamWriter>

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
    void iDoExport(QModelIndexList&) const;

private:
    void writeAdifField(QString, QVariant, QXmlStreamWriter&) const;
    QString getModeEnum(const QString&) const;
};

}
}


#endif // ADIFEXPORTER_H
