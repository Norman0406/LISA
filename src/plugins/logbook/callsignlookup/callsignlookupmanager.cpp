#include "callsignlookupmanager.h"
#include "callsignlookupqrzcom.h"

#include <coreplugin/icore.h>

using namespace Logbook::Internal;

CallsignLookupManager::CallsignLookupManager(QObject* parent)
    : QObject(parent),
      m_logbookForm(0)
{
    m_lookupServices.push_back(new CallsignLookupQRZcom(this));

    foreach (CallsignLookup* service, m_lookupServices) {
        connect(service, &CallsignLookup::callsignRetrieved, this, &CallsignLookupManager::callsignRetrieved);
    }

    loadSettings();
}

CallsignLookupManager::~CallsignLookupManager()
{
    foreach (CallsignLookup* service, m_lookupServices)
        delete service;
}

void CallsignLookupManager::registerLogbookForm(LogbookFormDialog* logbookForm)
{
    m_logbookForm = logbookForm;
}

void CallsignLookupManager::unregisterLogbookForm()
{
    m_logbookForm = 0;
}

const QList<CallsignLookup*> CallsignLookupManager::getServices() const
{
    return m_lookupServices;
}

CallsignLookup* CallsignLookupManager::getService(CallsignData::CallsignService serviceId)
{
    foreach (CallsignLookup* service, m_lookupServices) {
        if (service->getService() == serviceId)
            return service;
    }

    return 0;
}

void CallsignLookupManager::loadSettings()
{
    QSettings* settings = Core::ICore::settings();

    settings->beginGroup(QLatin1String("CallsignLookup"));
    foreach (CallsignLookup* service, m_lookupServices) {
        settings->beginGroup(CallsignData::getServiceString(service->getService()));

        if (service->getService() == CallsignData::CS_QRZCOM) {
            CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);
            qrzLookup->setUsername(settings->value(QLatin1String("Username")).toString());
            qrzLookup->setPassword(settings->value(QLatin1String("Password")).toString());
        }

        settings->endGroup();
    };
    settings->endGroup();
}

void CallsignLookupManager::saveSettings()
{
    QSettings* settings = Core::ICore::settings();

    settings->beginGroup(QLatin1String("CallsignLookup"));
    foreach (CallsignLookup* service, m_lookupServices) {
        settings->beginGroup(CallsignData::getServiceString(service->getService()));

        if (service->getService() == CallsignData::CS_QRZCOM) {
            CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);
            settings->setValue(QLatin1String("Username"), qrzLookup->getUsername());
            settings->setValue(QLatin1String("Password"), qrzLookup->getPassword());
        }

        settings->endGroup();
    };
    settings->endGroup();
}

void CallsignLookupManager::lookup(QString callsign)
{
    foreach (CallsignLookup* service, m_lookupServices) {
        service->lookup(callsign);
    }
}

void CallsignLookupManager::callsignRetrieved(CallsignData callsignData)
{
    // TODO: set the right entries on m_logbookForm

    if (m_logbookForm) {
        switch (callsignData.getService()) {
        case CallsignData::CS_QRZCOM:
            break;
        }
    }
}
