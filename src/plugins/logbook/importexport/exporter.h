#ifndef EXPORTER_H
#define EXPORTER_H

#include <QObject>
#include <QString>

namespace Logbook {
namespace Internal {

class Exporter
        : public QObject
{
    Q_OBJECT

public:
    ~Exporter();

    virtual QString getType() const = 0;

    void exportTo(const QString&);

protected:
    Exporter(QObject* parent = 0);

    virtual void iExportTo(const QString&) const = 0;
};

}
}

#endif // EXPORTER_H
