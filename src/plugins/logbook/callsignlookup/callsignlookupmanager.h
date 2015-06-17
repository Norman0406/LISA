#ifndef CALLSIGNLOOKUPMANAGER_H
#define CALLSIGNLOOKUPMANAGER_H

#include <QObject>

#include "callsigndata.h"

namespace Logbook {
namespace Internal {

class CallsignLookup;
class LogbookEntryPane;

class CallsignLookupManager
        : public QObject
{
    Q_OBJECT

public:
    CallsignLookupManager(QObject* parent);
    ~CallsignLookupManager();

    void registerEntryPane(LogbookEntryPane*);
    void unregisterEntryPane();

    const QList<CallsignLookup*> getServices() const;
    CallsignLookup* getService(CallsignData::CallsignService);

    void loadSettings();
    void saveSettings();

public slots:
    void lookup(QString);

private slots:
    void callsignRetrieved(CallsignData);

private:
    QList<CallsignLookup*> m_lookupServices;
    LogbookEntryPane* m_entryPane;
};

}
}

#endif // CALLSIGNLOOKUPMANAGER_H
