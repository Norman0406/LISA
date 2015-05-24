#include "logbookformdialog.h"
#include "ui_logbookformdialog.h"
#include "logbookwindow.h"

#include <QMap>

using namespace Logbook::Internal;

LogbookFormDialog::LogbookFormDialog(QWidget* parent, LogbookWindow* window)
{
    m_ui = new Ui::LogbookFormDialog();
    m_ui->setupUi(this);
    m_window = window;
    m_data = new QMap<QString, QString>;
    connect(m_ui->pushButtonSubmitLogbookForm, &QPushButton::clicked, this, &LogbookFormDialog::on_pushButtonSubmitLogbookForm_clicked);

}

LogbookFormDialog::~LogbookFormDialog()
{
    delete m_window;
    delete m_data;
}

void Logbook::Internal::LogbookFormDialog::on_pushButtonSubmitLogbookForm_clicked()
{
    m_data->insert(QString::fromLatin1("Callsign"), m_ui->lineEditCallsign->text());
    m_window->addQso(m_data);
}
