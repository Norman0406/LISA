#include "callsignlookupmanager.h"
#include "callsignlookupqrzcom.h"
#include "../logbookformdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/settingsdatabase.h>

using namespace Logbook::Internal;

CallsignLookupManager::CallsignLookupManager(QObject* parent)
    : QObject(parent),
      m_logbookForm(0),
      m_lookupEnabled(true)
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

void CallsignLookupManager::setEnabled(bool enabled)
{
    m_lookupEnabled = enabled;
}

bool CallsignLookupManager::isEnabled() const
{
    return m_lookupEnabled;
}

void CallsignLookupManager::loadSettings()
{
    Core::SettingsDatabase* settings = Core::ICore::settingsDatabase();

    settings->beginGroup(QLatin1String("CallsignLookup"));
    m_lookupEnabled = settings->value(QLatin1String("Enabled")).toBool();
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
    settings->setValue(QLatin1String("Enabled"), m_lookupEnabled);
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
    if (m_lookupEnabled) {
        foreach (CallsignLookup* service, m_lookupServices) {
            service->lookup(callsign);
        }
    }
}

void CallsignLookupManager::callsignRetrieved(CallsignData callsignData)
{    if (m_logbookForm) {
        switch (callsignData.getService()) {
        case CallsignData::CS_QRZCOM:
            QString name = QString(QLatin1String("%1 %2")).arg(callsignData.getField(QLatin1String("fname"))).arg(callsignData.getField(QLatin1String("name")));
            m_logbookForm->setName(name);
            break;
        }
    }
}
