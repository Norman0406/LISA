#ifndef LOGBOOKFORM_H
#define LOGBOOKFORM_H

#include <coreplugin/ioutputpane.h>

namespace Logbook {
namespace Internal {

class LogbookFormDialog;
class LogbookWindow;

class LogbookForm : public Core::IOutputPane
{
    Q_OBJECT

public:
    LogbookForm(QWidget*, LogbookWindow* window);
    virtual ~LogbookForm();

    QWidget *outputWidget(QWidget *);
    QList<QWidget*> toolBarWidgets() const;

    QString displayName() const { return tr("Logbook Entry"); }
    int priorityInStatusBar() const { return 1; }

    void visibilityChanged(bool visible);

    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;

private:
    LogbookFormDialog* m_dialog;
};

}
}


#endif // LOGBOOKFORM_H
