#ifndef CALLSIGNLOOKUPSETTINGSPAGE_H
#define CALLSIGNLOOKUPSETTINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

namespace Logbook {
namespace Internal {

namespace Ui {
    class CallsignLookupSettingsPage;
}

class CallsignLookupManager;

class CallsignLookupSettingsPage
        : public Core::IOptionsPage
{
public:
    CallsignLookupSettingsPage(CallsignLookupManager*);

    bool matches(const QString& searchKeyWord) const;
    QWidget* widget();
    void apply();
    void finish();

private:
    Ui::CallsignLookupSettingsPage* m_page;
    QPointer<QWidget> m_widget;
    CallsignLookupManager* m_manager;
};

}
}

#endif // CALLSIGNLOOKUPSETTINGSPAGE_H