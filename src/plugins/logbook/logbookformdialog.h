#ifndef LOGBOOKFORMDIALOG_H
#define LOGBOOKFORMDIALOG_H

#include <QWidget>

namespace Logbook {
namespace Internal {

namespace Ui {
    class LogbookFormDialog;
}

class LogbookFormDialog
        : public QWidget
{
public:
    LogbookFormDialog();

private:
    Ui::LogbookFormDialog* m_ui;
};

}
}

#endif // LOGBOOKFORMDIALOG_H
