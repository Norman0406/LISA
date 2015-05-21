#include "logbookformdialog.h"
#include "ui_logbookformdialog.h"

using namespace Logbook::Internal;

LogbookFormDialog::LogbookFormDialog()
{
    m_ui = new Ui::LogbookFormDialog();
    m_ui->setupUi(this);
}

