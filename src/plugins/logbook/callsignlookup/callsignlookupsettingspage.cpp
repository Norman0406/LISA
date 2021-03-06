#include "callsignlookupsettingspage.h"
#include "../logbookconstants.h"
#include "ui_callsignlookupsettingspage.h"
#include "callsignlookupmanager.h"
#include "callsignlookupqrzcom.h"

using namespace Logbook::Internal;

CallsignLookupSettingsPage::CallsignLookupSettingsPage(CallsignLookupManager* manager)
    : m_manager(manager),
      m_page(0),
      m_widget(0)
{
    setId(Logbook::Constants::SETTINGS_ID_LOGBOOK);
    setDisplayName(tr("Callsign Lookup"));
    setCategory(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK);
    setDisplayCategory(QCoreApplication::translate("Logbook",
        Logbook::Constants::SETTINGS_TR_CATEGORY_LOGBOOK));
    setCategoryIcon(QLatin1String(Logbook::Constants::SETTINGS_CATEGORY_LOGBOOK_ICON));
}

bool CallsignLookupSettingsPage::matches(const QString& searchKeyWord) const
{
    return false;
}

QWidget* CallsignLookupSettingsPage::widget()
{
    if (!m_widget) {
        m_page = new Ui::CallsignLookupSettingsPage();
        m_widget = new QWidget;
        m_page->setupUi(m_widget);

        // get services and read data
        const QList<CallsignLookup*> services = m_manager->getServices();
        foreach (CallsignLookup* service, services) {
            if (service->getService() == CallsignData::CS_QRZCOM) {
                CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(service);

                // the toggled signal is only changed when the state changes, so force a change here
                m_page->chkQRZEnabled->setChecked(!qrzLookup->isEnabled());
                m_page->chkQRZEnabled->setChecked(qrzLookup->isEnabled());

                m_page->edQRZUsername->setText(qrzLookup->getUsername());
                m_page->edQRZPassword->setText(qrzLookup->getPassword());
            }
        }
    }

    return m_widget;
}

void CallsignLookupSettingsPage::apply()
{
    CallsignLookupQRZcom* qrzLookup = qobject_cast<CallsignLookupQRZcom*>(m_manager->getService(CallsignData::CS_QRZCOM));
    if (qrzLookup) {
        qrzLookup->setEnabled(m_page->chkQRZEnabled->isChecked());
        qrzLookup->setUsername(m_page->edQRZUsername->text());
        qrzLookup->setPassword(m_page->edQRZPassword->text());
    }

    m_manager->saveSettings();
}

void CallsignLookupSettingsPage::finish()
{
    delete m_widget;
    m_widget = 0;
    delete m_page;
    m_page = 0;
}
