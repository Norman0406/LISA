#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>
#include <QString>

namespace Logbook {
namespace Internal {

class Importer
        : public QObject
{
    Q_OBJECT

public:
    ~Importer();

    virtual QString getType() const = 0;

    void importFrom(const QString&);

protected:
    Importer(QObject* parent = 0);

    virtual void iImportFrom(const QString&) const = 0;
};

}
}


#endif // IMPORTER_H
