#include "logbookformdialog.h"
#include "ui_logbookformdialog.h"
#include "qsoentry.h"
#include "coreplugin/icore.h"
#include "callsignlookup/callsignlookupmanager.h"
#include "logbookwindow.h"

#include <QMap>
#include <QIntValidator>
#include <QTimer>
#include <QKeyEvent>
#include <QSettings>
#include <QSqlRelationalTableModel>

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
    m_settings = Core::ICore::settings();

    setupWidgets();
    loadDefaults();

    connect(m_qtimer, &QTimer::timeout, this, &LogbookFormDialog::handleTimer);
    connect(m_ui->pushButtonSubmitLogbookForm, &QPushButton::clicked, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditCallsign, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditCallsign, &QLineEdit::returnPressed, this, &LogbookFormDialog::triggerLookup);
    connect(m_ui->lineEditFrequency, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditName, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstSend, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    connect(m_ui->lineEditRstRcvd, &QLineEdit::returnPressed, this, &LogbookFormDialog::validateInput);
    m_qtimer->start(1000);
}

LogbookFormDialog::~LogbookFormDialog()
{
    delete m_data;
    delete m_rstValidator;
    delete m_qsoEntry;
    delete m_qtimer;
}

void LogbookFormDialog::on_pushButtonSubmitLogbookForm_clicked()
{
    m_data->insert(QString::fromLatin1("CallsignFrom"), m_settings->value(QString::fromLatin1("logbook/callsign")).toString());
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

void LogbookFormDialog::triggerLookup()
{
    emit lookupCallsign(m_ui->lineEditCallsign->text());
}

void LogbookFormDialog::validateInput()
{
    //TODO: sanitation
    if(m_ui->lineEditCallsign->text().isEmpty())
    {
        m_ui->lineEditCallsign->setFocus();
        m_ui->lineEditCallsign->setStyleSheet(QString::fromLatin1("border: 1px solid red"));
    }
    else
    {
        on_pushButtonSubmitLogbookForm_clicked();
    }
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
    //emit startTimer();
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

void LogbookFormDialog::updateFrequency()
{
    QString band = m_ui->comboBoxBand->currentText();
    m_qsoEntry->setFrequencyByBand(QsoEntry::getBandByString(band));

    m_ui->lineEditFrequency->setText(QString::fromLatin1("%1").arg(m_qsoEntry->getFrequency()));
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

void LogbookFormDialog::tableViewDoubleClicked()
{
    QTableView* view  = m_window->getLogbookView();
    QItemSelection selection = view->selectionModel()->selection();
    QSqlRelationalTableModel* model = qobject_cast<QSqlRelationalTableModel*>(view->model());
    if(model)
    {
        if(selection.size() < 2)
        {
            int rowIndex = view->selectionModel()->currentIndex().row();
            qDebug() << isFormEmpty();
            if(isFormEmpty())
            {
                m_ui->lineEditCallsign->setText(model->data(model->index(rowIndex, 3)).toString());
                m_ui->lineEditName->setText(model->data(model->index(rowIndex, 4)).toString());
                m_ui->lineEditFrequency->setText(model->data(model->index(rowIndex, 6)).toString());
                m_ui->lineEditRstSend->setText(model->data(model->index(rowIndex, 8)).toString());
                m_ui->lineEditRstRcvd->setText(model->data(model->index(rowIndex, 9)).toString());
            }
        }
    }
}

bool LogbookFormDialog::isFormEmpty()
{
    if(m_ui->lineEditCallsign->text().isEmpty() && m_ui->lineEditName->text().isEmpty())
        return true;
    else false;
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

    m_ui->lineEditRstSend->setText(QString::fromLatin1("599"));
    m_ui->lineEditRstRcvd->setText(QString::fromLatin1("599"));

    updateFrequency();
    connect(m_ui->comboBoxBand, &QComboBox::currentTextChanged, this, &LogbookFormDialog::updateFrequency);
    QTableView* logbook_view = m_window->getLogbookView();
    connect(logbook_view, &QTableView::doubleClicked, this, &LogbookFormDialog::tableViewDoubleClicked);

    QList<QLineEdit*> widgets = m_ui->layoutWidget->findChildren<QLineEdit*>();
    foreach (QLineEdit* edit, widgets) {
        if (edit) {
            connect(edit, &QLineEdit::textChanged, this, &LogbookFormDialog::stopTimer);
            connect(edit, &QLineEdit::textChanged, this, &LogbookFormDialog::convertInputToUppercase);
            edit->installEventFilter(this);
        }
    }

}
