#include "callsignlookupmanager.h"
#include "callsignlookupqrzcom.h"
#include "../logbookentrypane.h"

#include <coreplugin/icore.h>
#include <coreplugin/settingsdatabase.h>

using namespace Logbook::Internal;

CallsignLookupManager::CallsignLookupManager(QObject* parent)
    : QObject(parent),
      m_entryPane(0)
{
    m_lookupServices.push_back(new CallsignLookupQRZcom(this));

    foreach (CallsignLookup* service, m_lookupServices) {
        connect(service, &CallsignLookup::callsignRetrieved, this, &CallsignLookupManager::callsignRetrieved);
        connect(service, &CallsignLookup::error, this, &CallsignLookupManager::error);
    }

    loadSettings();
}

CallsignLookupManager::~CallsignLookupManager()
{
    foreach (CallsignLookup* service, m_lookupServices)
        delete service;
}

void CallsignLookupManager::registerEntryPane(LogbookEntryPane* entryPane)
{
    m_entryPane = entryPane;
}

void CallsignLookupManager::unregisterEntryPane()
{
    m_entryPane = 0;
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
    Core::SettingsDatabase* settings = Core::ICore::settingsDatabase();

    settings->beginGroup(QLatin1String("CallsignLookup"));
    foreach (CallsignLookup* service, m_lookupServices) {
        settings->beginGroup(CallsignData::getServiceString(service->getService()));

        if (service->getService() == CallsignData::CS_QRZCOM) {
            CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);
            qrzLookup->setEnabled(settings->value(QLatin1String("Enabled")).toBool());
            qrzLookup->setUsername(settings->value(QLatin1String("Username")).toString());
            qrzLookup->setPassword(settings->value(QLatin1String("Password")).toString());
        }

        settings->endGroup();
    };
    settings->endGroup();
}

void CallsignLookupManager::saveSettings()
{
    Core::SettingsDatabase* settings = Core::ICore::settingsDatabase();

    settings->beginGroup(QLatin1String("CallsignLookup"));
    foreach (CallsignLookup* service, m_lookupServices) {
        settings->beginGroup(CallsignData::getServiceString(service->getService()));

        if (service->getService() == CallsignData::CS_QRZCOM) {
            CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);
            settings->setValue(QLatin1String("Enabled"), qrzLookup->isEnabled());
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
    if (m_entryPane) {
        switch (callsignData.getService()) {
        case CallsignData::CS_QRZCOM:
            QString name = QString(QLatin1String("%1 %2")).arg(callsignData.getField(QLatin1String("fname"))).arg(callsignData.getField(QLatin1String("name")));
            m_entryPane->setUiName(name);
            break;
        }
    }
}

void CallsignLookupManager::error(QString errorString)
{
    qWarning() << errorString;
}
