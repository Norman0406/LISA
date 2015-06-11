#ifndef LOGBOOKENTRYPANE_H
#define LOGBOOKENTRYPANE_H

#include <coreplugin/ioutputpane.h>
#include "qsoentry.h"

namespace Logbook {
namespace Internal {

namespace Ui {
class LogbookEntryWidget;
}

class LogbookEntryPane
        : public Core::IOutputPane
{
    Q_OBJECT

public:
    LogbookEntryPane(QWidget* parent);
    virtual ~LogbookEntryPane();

    QWidget *outputWidget(QWidget* );
    QList<QWidget*> toolBarWidgets() const;

    QString displayName() const { return tr("Logbook Entry 2"); }
    int priorityInStatusBar() const { return 1; }

    void visibilityChanged(bool visible);

    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;

public slots:
    void qsoSelected(const QsoEntry&);

signals:
    void qsoModified(const QsoEntry&);

private:
    Ui::LogbookEntryWidget* m_ui;
    QWidget* m_widget;
};

}
}

#endif // LOGBOOKENTRYPANE_H
