#ifndef CALLSIGNLOOKUPSETTINGSPAGE_H
#define CALLSIGNLOOKUPSETTINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QPointer>

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

private slots:
    void on_checkBox_clicked(bool checked);

private:
    Ui::CallsignLookupSettingsPage* m_page;
    QWidget* m_widget;
    CallsignLookupManager* m_manager;
};

}
}

#endif // CALLSIGNLOOKUPSETTINGSPAGE_H
