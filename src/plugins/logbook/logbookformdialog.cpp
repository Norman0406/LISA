#include "logbookformdialog.h"
#include "ui_logbookformdialog.h"
#include "qsoentry.h"
#include "logbookwindow.h"

#include <QMap>
#include <QIntValidator>
#include <QTimer>
#include <QKeyEvent>

using namespace Logbook::Internal;

LogbookFormDialog::LogbookFormDialog(QWidget* parent, LogbookWindow* window)
{
    m_ui = new Ui::LogbookFormDialog();
    m_ui->setupUi(this);
    m_window = window;
    m_data = new QMap<QString, QString>;
    m_rstValidator = new QIntValidator(0, 599, this);
    m_qsoEntry = new QsoEntry();
    m_qtimer = new QTimer(this);

    setupWidgets();
    loadDefaults();

    connect(m_qtimer, &QTimer::timeout, this, &LogbookFormDialog::handleTimer);
    connect(m_ui->pushButtonSubmitLogbookForm, &QPushButton::clicked, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditCallsign, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditFrequency, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditName, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstSend, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstRcvd, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    m_qtimer->start(1000);
}

LogbookFormDialog::~LogbookFormDialog()
{
    delete m_window;
    delete m_data;
    delete m_data;
    delete m_rstValidator;
    delete m_qsoEntry;
    delete m_qtimer;
}

void LogbookFormDialog::on_pushButtonSubmitLogbookForm_clicked()
{
    m_data->insert(QString::fromLatin1("Datetime"), m_ui->dateTimeEdit->dateTime().toString());
    m_data->insert(QString::fromLatin1("Callsign"), m_ui->lineEditCallsign->text());
    m_data->insert(QString::fromLatin1("Name"), m_ui->lineEditName->text());
    m_data->insert(QString::fromLatin1("Mode"), m_ui->comboBoxMode->currentText());
    m_data->insert(QString::fromLatin1("Frequency"), m_ui->lineEditFrequency->text());
    m_data->insert(QString::fromLatin1("Band"), m_ui->comboBoxBand->currentText());
    m_data->insert(QString::fromLatin1("RSTSend"), m_ui->lineEditRstSend->text());
    m_data->insert(QString::fromLatin1("RSTRcvd"), m_ui->lineEditRstRcvd->text());

    m_window->addQso(m_data);
}

void LogbookFormDialog::handleTimer()
{
    m_ui->dateTimeEdit->setTime(QDateTime::currentDateTime().time());
}

void LogbookFormDialog::stopTimer()
{
    m_qtimer->stop();
}

void LogbookFormDialog::startTimer()
{
    m_qtimer->start(1000);
}

void LogbookFormDialog::validateInput()
{
    //TODO: sanitation
    on_pushButtonSubmitLogbookForm_clicked();
    m_qtimer->start(1000);
}

void LogbookFormDialog::clearForm()
{
    QList<QLineEdit*> widgets = m_ui->layoutWidget->findChildren<QLineEdit*>();
    foreach (QLineEdit* edit, widgets) {
        if (edit) {
            edit->setText(QString::fromLatin1(""));
        }
    }
    emit startTimer();
    m_ui->lineEditCallsign->setFocus();
}

void LogbookFormDialog::convertInputToUppercase()
{
    QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
    if(sender)
    {
        QString text = sender->text();
        text = text.toUpper();
        sender->setText(text);
    }
}

bool LogbookFormDialog::eventFilter(QObject *target, QEvent *event)
{
    if (target == m_ui->lineEditCallsign || target == m_ui->lineEditFrequency
            || target == m_ui->lineEditName || target == m_ui->lineEditRstRcvd
            || target == m_ui->lineEditRstSend) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            if (keyEvent && keyEvent->key() == Qt::Key_Escape) {
                clearForm();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}

void LogbookFormDialog::loadDefaults()
{
    m_ui->lineEditRstSend->setValidator(m_rstValidator);
    m_ui->lineEditRstRcvd->setValidator(m_rstValidator);
}

void LogbookFormDialog::setupWidgets()
{
    m_modes.append(QString::fromLatin1("AM"));
    m_modes.append(QString::fromLatin1("FM"));
    m_modes.append(QString::fromLatin1("SSB"));
    m_modes.append(QString::fromLatin1("USB"));
    m_modes.append(QString::fromLatin1("LSB"));
    m_modes.append(QString::fromLatin1("PSK"));
    m_modes.append(QString::fromLatin1("RTTY"));

    m_ui->comboBoxMode->addItems(m_modes);

    m_bands.append(QString::fromLatin1("160M"));
    m_bands.append(QString::fromLatin1("80M"));
    m_bands.append(QString::fromLatin1("40M"));
    m_bands.append(QString::fromLatin1("30M"));
    m_bands.append(QString::fromLatin1("20M"));
    m_bands.append(QString::fromLatin1("17M"));
    m_bands.append(QString::fromLatin1("15M"));
    m_bands.append(QString::fromLatin1("12M"));
    m_bands.append(QString::fromLatin1("10M"));
    m_bands.append(QString::fromLatin1("6M"));
    m_bands.append(QString::fromLatin1("2M"));
    m_bands.append(QString::fromLatin1("70CM"));
    m_bands.append(QString::fromLatin1("23CM"));
    m_bands.append(QString::fromLatin1("13CM"));

    m_ui->comboBoxBand->addItems(m_bands);

    m_ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    m_ui->dateTimeEdit->setTimeSpec(Qt::UTC);

    QList<QLineEdit*> widgets = m_ui->layoutWidget->findChildren<QLineEdit*>();
    foreach (QLineEdit* edit, widgets) {
        if (edit) {
            connect(edit, &QLineEdit::textChanged, this, &LogbookFormDialog::stopTimer);
            connect(edit, &QLineEdit::textChanged, this, &LogbookFormDialog::convertInputToUppercase);
            edit->installEventFilter(this);
        }
    }

}
