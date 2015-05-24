#ifndef CALLSIGNLOOKUP_H
#define CALLSIGNLOOKUP_H

#include "callsigndata.h"

#include <QObject>

namespace Logbook {
namespace Internal {

class CallsignLookup
        : public QObject
{
    Q_OBJECT

public:
    ~CallsignLookup();

    bool lookup(QString);

protected:
    CallsignLookup();

    virtual bool iLookup(QString) = 0;
};

}
}

#endif // CALLSIGNLOOKUP_H
