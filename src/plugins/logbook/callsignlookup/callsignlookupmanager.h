#ifndef CALLSIGNLOOKUPMANAGER_H
#define CALLSIGNLOOKUPMANAGER_H

#include <QObject>

#include "callsigndata.h"

namespace Logbook {
namespace Internal {

class CallsignLookup;
class LogbookFormDialog;

class CallsignLookupManager
        : public QObject
{
    Q_OBJECT

public:
    CallsignLookupManager(QObject* parent);
    ~CallsignLookupManager();

    void registerLogbookForm(LogbookFormDialog*);
    void unregisterLogbookForm();

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
    LogbookFormDialog* m_logbookForm;
};

}
}

#endif // CALLSIGNLOOKUPMANAGER_H
