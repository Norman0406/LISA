#ifndef LOGBOOKFORM_H
#define LOGBOOKFORM_H

#include <coreplugin/ioutputpane.h>
#include <QComboBox>
#include "profiledata.h"

namespace Logbook {
namespace Internal {

class LogbookMode;
class LogbookFormDialog;
class LogbookWindow;

class LogbookForm : public Core::IOutputPane
{
    Q_OBJECT

public:
    LogbookForm(QWidget* parent, LogbookMode* mode, LogbookWindow *window);
    virtual ~LogbookForm();

    LogbookFormDialog* getDialog() const;

    QWidget *outputWidget(QWidget* );
    QList<QWidget*> toolBarWidgets() const;

    QString displayName() const { return tr("Logbook Entry"); }
    int priorityInStatusBar() const { return 1; }

    void visibilityChanged(bool visible);

    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;

private slots:
    void updateProfiles(const QList<ProfileData>&);
    void selectedProfileChanged(int);

private:
    LogbookMode* m_mode;
    LogbookWindow* m_window;
    LogbookFormDialog* m_dialog;
    QList<QWidget*> m_toolbarWidgets;
    QComboBox* m_profileList;
    QList<QUuid> m_profileUuids;
    QUuid m_selectedProfile;
};

}
}

#endif // LOGBOOKFORM_H
