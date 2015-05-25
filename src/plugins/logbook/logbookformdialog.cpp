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

    connect(m_ui->pushButtonSubmitLogbookForm, &QPushButton::clicked, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditCallsign, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditBand, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditFrequency, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditMode, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditName, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstSend, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstRcvd, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);

}

LogbookFormDialog::~LogbookFormDialog()
{
    delete m_data;
}

void LogbookFormDialog::on_pushButtonSubmitLogbookForm_clicked()
{
    m_data->insert(QString::fromLatin1("Callsign"), m_ui->lineEditCallsign->text());
    m_data->insert(QString::fromLatin1("Name"), m_ui->lineEditName->text());
    m_data->insert(QString::fromLatin1("Mode"), m_ui->lineEditMode->text());
    m_data->insert(QString::fromLatin1("Frequency"), m_ui->lineEditFrequency->text());
    m_data->insert(QString::fromLatin1("Band"), m_ui->lineEditBand->text());
    m_data->insert(QString::fromLatin1("RSTSend"), m_ui->lineEditRstSend->text());
    m_data->insert(QString::fromLatin1("RSTRcvd"), m_ui->lineEditRstRcvd->text());

    m_window->addQso(m_data);
}

void LogbookFormDialog::validateInput()
{
    on_pushButtonSubmitLogbookForm_clicked();
    QList<QLineEdit*> widgets = m_ui->layoutWidget->findChildren<QLineEdit*>();
    foreach (QLineEdit* edit, widgets) {
        if (edit) {
            edit->setText(QString::fromLatin1(""));
        }
    }
    qDebug() << "Input validation happens here";

    //TODO: Date time
    //TODO: Logbook
    //
}

void LogbookFormDialog::convertInputToUppercase()
{

}
