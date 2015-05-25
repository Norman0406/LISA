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

    // TODO: set additional information from settings
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

void CallsignLookupManager::loadSettings()
{
    QSettings* settings = Core::ICore::settings();
}

void CallsignLookupManager::saveSettings()
{
    QSettings* settings = Core::ICore::settings();

    settings->beginGroup(QString::fromLatin1("CallsignLookup"));
    foreach (CallsignLookup* service, m_lookupServices) {
        settings->beginGroup(CallsignData::getServiceString(service->getService()));

        if (service->getService() == CallsignData::CS_QRZCOM) {
            CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);
            settings->setValue(QString::fromLatin1("Username"), qrzLookup->getUsername());
            settings->setValue(QString::fromLatin1("Password"), qrzLookup->getPassword());
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
    // TODO: set the right entries on logbookForm

    if (m_logbookForm) {
        switch (callsignData.getService()) {
        case CallsignData::CS_QRZCOM:
            break;
        }
    }
}
