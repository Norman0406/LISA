#ifndef LOGBOOKFORMDIALOG_H
#define LOGBOOKFORMDIALOG_H

#include <QWidget>

namespace Logbook {
namespace Internal {

namespace Ui {
    class LogbookFormDialog;
}
class LogbookWindow;

class LogbookFormDialog
        : public QWidget
{
public:
    LogbookFormDialog(QWidget* parent, LogbookWindow* window);
signals:
    void QsoAdded();

private:
    Ui::LogbookFormDialog* m_ui;
};

}
}

#endif // LOGBOOKFORMDIALOG_H
