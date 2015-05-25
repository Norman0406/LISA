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
    CallsignData::CallsignService getService() const;

protected:
    CallsignLookup(CallsignData::CallsignService);

    virtual bool iLookup(QString) = 0;

private:
    const CallsignData::CallsignService m_service;
};

}
}

#endif // CALLSIGNLOOKUP_H
