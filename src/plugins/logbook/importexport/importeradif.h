#ifndef IMPORTERADIF_H
#define IMPORTERADIF_H

#include "importer.h"

namespace Logbook {
namespace Internal {

class ImporterAdif
        : public Importer
{
public:
    ImporterAdif(QObject* parent = 0);
    ~ImporterAdif();

    static QString getTypeStatic();
    QString getType() const;

protected:
    void iImportFrom(const QString&) const;
};

}
}

#endif // IMPORTERADIF_H
