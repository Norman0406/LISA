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
    void setEnabled(bool);
    bool isEnabled() const;
    CallsignData::CallsignService getService() const;

signals:
    void callsignRetrieved(CallsignData);

protected:
    CallsignLookup(QObject* parent, CallsignData::CallsignService);

    virtual bool iLookup(QString) = 0;

private:
    const CallsignData::CallsignService m_service;
    bool m_enabled;
};

}
}

#endif // CALLSIGNLOOKUP_H
