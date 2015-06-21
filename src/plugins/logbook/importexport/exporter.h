#ifndef EXPORTER_H
#define EXPORTER_H

#include <QObject>
#include <QString>
#include <QModelIndexList>

namespace Logbook {
namespace Internal {

class Exporter
        : public QObject
{
    Q_OBJECT

public:
    ~Exporter();

    virtual QString getType() const = 0;

    void doExport(QModelIndexList&);

protected:
    Exporter(QObject* parent = 0);

    virtual void iDoExport(QModelIndexList&) const = 0;
};

}
}

#endif // EXPORTER_H
