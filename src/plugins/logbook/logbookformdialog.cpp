#include "logbookformdialog.h"
#include "ui_logbookformdialog.h"
#include "logbookwindow.h"

using namespace Logbook::Internal;

LogbookFormDialog::LogbookFormDialog(QWidget* parent, LogbookWindow* window)
{
    m_ui = new Ui::LogbookFormDialog();
    m_ui->setupUi(this);

    connect(m_ui->pushButtonSubmitLogbookForm, &QPushButton::clicked, window, &LogbookWindow::addQso);
}

