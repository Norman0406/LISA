#ifndef CALLSIGNLOOKUPMANAGER_H
#define CALLSIGNLOOKUPMANAGER_H

#include <QObject>

namespace Logbook {
namespace Internal {

class CallsignLookupManager
        : public QObject
{
    Q_OBJECT

public:
    CallsignLookupManager(QObject* parent);
    ~CallsignLookupManager();
};

}
}

#endif // CALLSIGNLOOKUPMANAGER_H
