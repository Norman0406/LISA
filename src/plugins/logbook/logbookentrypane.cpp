#include "logbookentrypane.h"
#include "ui_logbookentrywidget.h"
#include "database.h"
#include "logbookentrydelegate.h"
#include "logbookmode.h"
#include "lineeditfrequency.h"
#include "countryfile.h"

#include <QMessageBox>

using namespace Logbook::Internal;

LogbookEntryPane::LogbookEntryPane(LogbookMode* mode, QWidget* parent)
    : Core::IOutputPane(parent),
      m_profile(0),
      m_mode(mode),
      m_model(0),
      m_editModel(0),
      m_selectedRow(-1),
      m_isDirty(false),
      m_dateTimerUpdate(false),
      m_hasPrevData(false),
      m_bandChanged(false)
{
    m_widget = new QWidget(parent);
    m_ui = new Ui::LogbookEntryWidget();
    m_ui->setupUi(m_widget);

    m_countryFile = new CountryFile();
    m_countryFile->open();

    m_itemDelegate = new LogbookEntryDelegate(m_ui, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->setOrientation(Qt::Horizontal);

    m_profileList = new QComboBox(parent);
    m_profileList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_toolbarWidgets.push_back(m_profileList);

    m_buttonAdd = new QPushButton(tr("Add"), m_widget);
    connect(m_buttonAdd, &QPushButton::clicked, this, &LogbookEntryPane::addQso);
    m_toolbarWidgets.push_back(m_buttonAdd);

    m_buttonClear = new QPushButton(tr("Clear"), m_widget);
    connect(m_buttonClear, &QPushButton::clicked, this, &LogbookEntryPane::clearQso);
    m_toolbarWidgets.push_back(m_buttonClear);

    m_buttonDelete = new QPushButton(tr("Delete"), m_widget);
    connect(m_buttonDelete, &QPushButton::clicked, this, &LogbookEntryPane::deleteQso);
    m_toolbarWidgets.push_back(m_buttonDelete);

    connect(m_profileList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
    connect(mode, &LogbookMode::profilesChanged, this, &LogbookEntryPane::updateProfiles);

    m_dateTimer = new QTimer(this);
    connect(m_dateTimer, &QTimer::timeout, this, &LogbookEntryPane::updateDateTime);

    m_dateTimer->setInterval(1000);
    m_dateTimer->setSingleShot(false);
    m_dateTimer->start();
    updateDateTime();

    initUiContents();
}

LogbookEntryPane::~LogbookEntryPane()
{
    foreach (QWidget* widget, m_toolbarWidgets)
        delete widget;

    delete m_mapper;
    delete m_itemDelegate;
    delete m_widget;
    delete m_ui;

    clearModel();
}

void LogbookEntryPane::initUiContents()
{
    connect(m_ui->lineEdit_QSO_CallsignTo, &QLineEdit::textChanged, this, &LogbookEntryPane::convertInputToUppercase);
    connect(m_ui->lineEdit_QSO_Frequency, &LineEditFrequency::frequencyChanged, this, &LogbookEntryPane::frequencyChanged);
    connect(m_ui->comboBox_QSO_Band, &QComboBox::currentTextChanged, this, &LogbookEntryPane::bandChanged);

    m_ui->lineEdit_QSO_CallsignTo->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_ui->lineEdit_QSO_CallsignTo->setValidator(new QRegExpValidator(QRegExp(QLatin1String("(?!/)([A-Z]|[a-z]|[0-9]|[/])*"))));
    connect(m_ui->lineEdit_QSO_CallsignTo, &QLineEdit::editingFinished, this, &LogbookEntryPane::callsignEntered);

    m_bands << QLatin1String("160m") <<
               QLatin1String("80m") <<
               QLatin1String("40m") <<
               QLatin1String("30m") <<
               QLatin1String("20m") <<
               QLatin1String("17m") <<
               QLatin1String("15m") <<
               QLatin1String("12m") <<
               QLatin1String("10m") <<
               QLatin1String("6m") <<
               QLatin1String("2m") <<
               QLatin1String("70cm") <<
               QLatin1String("23cm") <<
               QLatin1String("13cm");
    m_ui->comboBox_QSO_Band->addItems(m_bands);

    m_modes << QLatin1String("AM") <<
               QLatin1String("FM") <<
               QLatin1String("SSB") <<
               QLatin1String("USB") <<
               QLatin1String("LSB") <<
               QLatin1String("CW") <<
               QLatin1String("PSK") <<
               QLatin1String("QPSK") <<
               QLatin1String("RTTY") <<
               QLatin1String("DSTAR") <<
               QLatin1String("FAX") <<
               QLatin1String("JT65");
    m_modes.sort();
    m_ui->comboBox_QSO_Mode->addItems(m_modes);

    const QStringList& continents = m_countryFile->getContinents();
    m_ui->comboBox_Personal_Continent->addItems(continents);

    const QStringList& countries = m_countryFile->getCountries();
    m_ui->comboBox_Personal_Country->addItems(countries);

    // when the user changes the country, CQ and ITU Zone should change if not changed manually
    connect(m_ui->comboBox_Personal_Country, &QComboBox::currentTextChanged, [this](const QString name) {
        const Country* country = m_countryFile->getCountryFromName(name);
        if (country) {
            if (!hasManualInput(m_ui->lineEdit_Contest_CQZone))
                m_ui->lineEdit_Contest_CQZone->setText(QString(QLatin1String("%1")).arg(country->CQZone));
            if (!hasManualInput(m_ui->lineEdit_Contest_ITUZone))
                m_ui->lineEdit_Contest_ITUZone->setText(QString(QLatin1String("%1")).arg(country->ITUZone));
        }
    });

    connect(m_ui->comboBox_Personal_Callsign, &QComboBox::currentTextChanged, this, &LogbookEntryPane::callsignSelected);

    // connect to each widget, such that a signal is called whenever data changes
    QList<QWidget*> childWidgets = m_widget->findChildren<QWidget*>();
    foreach (QWidget* widget, childWidgets) {
        widget->setProperty("ManualInput", false);

        if (QLineEdit* w = qobject_cast<QLineEdit*>(widget))
            connect(w, SIGNAL(textChanged(QString)), this, SLOT(dirty()));
        else if (QDateTimeEdit* w = qobject_cast<QDateTimeEdit*>(widget))
            connect(w, SIGNAL(dateChanged(QDate)), this, SLOT(dirty()));
        else if (QSpinBox* w = qobject_cast<QSpinBox*>(widget))
            connect(w, SIGNAL(valueChanged(int)), this, SLOT(dirty()));
        else if (QComboBox* w = qobject_cast<QComboBox*>(widget))
            connect(w, SIGNAL(currentIndexChanged(int)), this, SLOT(dirty()));
        else if (QPlainTextEdit* w = qobject_cast<QPlainTextEdit*>(widget))
            connect(w, SIGNAL(textChanged()), this, SLOT(dirty()));
        else if (QCheckBox* w = qobject_cast<QCheckBox*>(widget))
            connect(w, SIGNAL(stateChanged(int)), this, SLOT(dirty()));
    }
}

QWidget* LogbookEntryPane::outputWidget(QWidget* parent)
{
    Q_UNUSED(parent)
    return m_widget;
}

QList<QWidget*> LogbookEntryPane::toolBarWidgets() const
{
    return m_toolbarWidgets;
}

void LogbookEntryPane::visibilityChanged(bool visible)
{
}

void LogbookEntryPane::setFocus()
{
}

bool LogbookEntryPane::hasFocus() const
{
}

bool LogbookEntryPane::canFocus() const
{
    return false;
}

void LogbookEntryPane::setModel(QSqlRelationalTableModel* model)
{
    m_model = model;

    // create a model with the same number of columns
    delete m_editModel;
    m_editModel = new QStandardItemModel();
    m_editModel->insertColumns(0, m_model->columnCount());
    m_editModel->submit();

    m_mapper->setModel(m_editModel);
    m_mapper->setItemDelegate(m_itemDelegate);

    m_mapper->addMapping(m_ui->dateTimeEdit_QSO_Date, m_model->fieldIndex(QLatin1String("DateTime")));
    m_mapper->addMapping(m_ui->dateTimeEdit_QSO_UTC, m_model->fieldIndex(QLatin1String("DateTime")));
    m_mapper->addMapping(m_ui->lineEdit_QSO_CallsignTo, m_model->fieldIndex(QLatin1String("Callsign")));
    //m_mapper->addMapping(m_ui->lineEdit_Personal_Name, 4);
    m_mapper->addMapping(m_ui->lineEdit_QSO_Frequency, m_model->fieldIndex(QLatin1String("Frequency")));
    m_mapper->addMapping(m_ui->comboBox_QSO_Mode, m_model->fieldIndex(QLatin1String("Mode")));
    m_mapper->addMapping(m_ui->lineEdit_QSO_RstSent, m_model->fieldIndex(QLatin1String("RstSent")));
    m_mapper->addMapping(m_ui->spinBox_QSO_SentNumber, m_model->fieldIndex(QLatin1String("RstSentNr")));
    m_mapper->addMapping(m_ui->lineEdit_QSO_RstRcvd, m_model->fieldIndex(QLatin1String("RstRcvd")));
    m_mapper->addMapping(m_ui->spinBox_QSO_RcvdNumber, m_model->fieldIndex(QLatin1String("RstRcvdNr")));
    m_mapper->addMapping(m_ui->plainTextEdit_QSO_Comment, m_model->fieldIndex(QLatin1String("Comment")));

    // when first loading a new model, a new empty qso is added
    newQso();
}

void LogbookEntryPane::clearModel()
{
    m_model = 0;
    delete m_editModel;
    m_editModel = 0;
    m_hasPrevData = false;
}

void LogbookEntryPane::setUiName(QString name)
{
    if (!hasManualInput(m_ui->lineEdit_Personal_Name))
        m_ui->lineEdit_Personal_Name->setText(name);
}

void LogbookEntryPane::setUiAddress(QString address)
{
    if (!hasManualInput(m_ui->plainTextEdit_Personal_Address))
        m_ui->plainTextEdit_Personal_Address->setPlainText(address);
}

void LogbookEntryPane::rowSelected(int row)
{
    // check if data has changed
    if (checkDirty())
        addQso();

    if (m_model) {
        m_selectedRow = row;

        // revert previous rows inserted
        m_editModel->revert();
        m_editModel->insertRow(0);
        for (int i = 1; i < m_model->columnCount(); i++) {
             QModelIndex editIndex = m_editModel->index(0, i);
             QModelIndex modelIndex = m_model->index(row, i);
             QVariant data = m_model->data(modelIndex);
             m_editModel->setData(editIndex, data);
        }

        m_mapper->setCurrentIndex(0);

        m_buttonDelete->setEnabled(true);
        m_buttonAdd->setText(tr("Modify"));

        callsignEntered();

        resetDirtyFlag();
    }
}

void LogbookEntryPane::addQso()
{
    if (m_model) {

        if (validateCallsign(m_ui->lineEdit_QSO_CallsignTo->text())) {
            m_mapper->submit();

            int rowIndex = m_selectedRow;

            if (rowIndex < 0) {
                rowIndex = m_model->rowCount();
                m_model->insertRow(rowIndex);
            }

            // update model (starting from 1 to skip the ID)
            for (int i = 1; i < m_model->columnCount(); i++) {
                QModelIndex editIndex = m_editModel->index(0, i);
                QModelIndex modelIndex = m_model->index(rowIndex, i);
                QVariant data = m_editModel->data(editIndex);
                m_model->setData(modelIndex, data);
            }
            m_model->submitAll();
            m_model->select();

            m_hasPrevData = true;

            newQso();
        }
    }
}

void LogbookEntryPane::newQso()
{
    if (m_model) {
        // clear entries
        if (m_editModel->rowCount() > 0)
            m_editModel->removeRow(0);

        m_buttonDelete->setEnabled(false);
        m_buttonAdd->setText(tr("Add"));

        m_ui->comboBox_Personal_Callsign->clear();
        m_ui->comboBox_Personal_Callsign->setEnabled(false);

        m_editModel->insertRow(0);
        m_mapper->setCurrentIndex(0);

        m_selectedRow = -1;

        if (!m_dateTimer->isActive()) {
            updateDateTime();
            m_dateTimer->start();
        }

        if (m_hasPrevData) {
            QSqlQuery query = m_model->query();

            // get data from the last added qso and set data to gui elements
            if (query.exec(QLatin1String("SELECT Frequency, Mode, Profile FROM logbook ORDER BY ID DESC LIMIT 1"))) {
                query.next();
                if (query.isValid()) {
                    double frequency = query.value(0).toDouble();
                    QString mode = query.value(1).toString();
                    //QString op = query.value(2).toString();

                    m_ui->comboBox_QSO_Mode->setCurrentText(mode);
                    m_ui->lineEdit_QSO_Frequency->setFrequency(frequency);
                }
            }
            else {
                qWarning() << "could not get last qso entries: " << query.lastError().text();
            }
        }

        resetDirtyFlag();
    }
}

void LogbookEntryPane::clearQso()
{
    if (m_model) {
        if (checkDirty())
            addQso();

        newQso();
    }
}

void LogbookEntryPane::deleteQso()
{
    if (m_model && m_selectedRow >= 0) {
        if (QMessageBox::warning(m_widget, tr("Really delete QSO?"),
                                 tr("Do you really want to delete this QSO? All entered data will be lost."),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            m_model->removeRow(m_selectedRow);
            m_model->submitAll();
            m_model->select();

            newQso();
        }
    }
}

void LogbookEntryPane::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    QDate date = m_ui->dateTimeEdit_QSO_Date->date();
    QTime time = m_ui->dateTimeEdit_QSO_UTC->time();
    QDateTime current(date, time);

    if (current != now) {
        bool previousDirtyFlag = m_isDirty;

        m_dateTimerUpdate = true;

        m_ui->dateTimeEdit_QSO_Date->setDate(now.date());
        m_ui->dateTimeEdit_QSO_UTC->setTime(now.time());

        m_dateTimerUpdate = false;

        m_isDirty = previousDirtyFlag;
    }
}

bool LogbookEntryPane::checkDirty()
{
    if (m_isDirty) {
        if (QMessageBox::warning(m_widget, tr("Unsaved changes"),
                                 tr("You have unsaved changes. Do you want to save the changed data?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            return true;
        }
    }

    return false;
}

void LogbookEntryPane::dirty()
{
    if (!m_isDirty) {
        // stop the date timer when a manual input occurred
        if (!m_dateTimerUpdate)
            m_dateTimer->stop();

        QWidget* widget = qobject_cast<QWidget*>(sender());
        if (widget)
            widget->setProperty("ManualInput", true);

        m_buttonAdd->setEnabled(true);

        m_isDirty = true;
    }
}

void LogbookEntryPane::resetDirtyFlag()
{
    QList<QWidget*> childWidgets = m_widget->findChildren<QWidget*>();
    foreach (QWidget* widget, childWidgets)
        widget->setProperty("ManualInput", false);

    m_buttonAdd->setEnabled(false);

    m_isDirty = false;
}

void LogbookEntryPane::convertInputToUppercase()
{
    QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
    if (sender) {
        QString text = sender->text();
        text = text.toUpper();
        int curPos = sender->cursorPosition();
        sender->setText(text);
        sender->setCursorPosition(curPos);
    }
}

void LogbookEntryPane::frequencyChanged(double frequency)
{
    QString bandString = bandFromFrequency(frequency);

    m_bandChanged = true;

    if (bandString.isEmpty()) {
        QPalette palette = m_ui->comboBox_QSO_Band->palette();
        palette.setColor(QPalette::Text, Qt::red);
        m_ui->comboBox_QSO_Band->setPalette(palette);

        // select nearest band
        double minFrq = -1;
        foreach (QString band, m_bands) {
            double frq = frequencyFromBand(band);
            if (minFrq < 0 || qAbs(frequency - frq) < qAbs(frequency - minFrq))
                minFrq = frq;
        }
        m_ui->comboBox_QSO_Band->setCurrentText(bandFromFrequency(minFrq));
    }
    else {
        QPalette palette = m_ui->comboBox_QSO_Band->palette();
        palette.setColor(QPalette::Text, Qt::black);
        m_ui->comboBox_QSO_Band->setPalette(palette);

        m_ui->comboBox_QSO_Band->setCurrentText(bandString);
    }

    m_bandChanged = false;
}

QString LogbookEntryPane::bandFromFrequency(double frequency) const
{
    QString bandString;
    if (inRange(frequency, 1.8, 2.0))
        bandString = QLatin1String("160m");
    else if (inRange(frequency, 3.5, 3.8))
        bandString = QLatin1String("80m");
    else if (inRange(frequency, 7, 7.2))
        bandString = QLatin1String("40m");
    else if (inRange(frequency, 10.1, 10.15))
        bandString = QLatin1String("30m");
    else if (inRange(frequency, 14, 14.35))
        bandString = QLatin1String("20m");
    else if (inRange(frequency, 18.068, 18.168))
        bandString = QLatin1String("17m");
    else if (inRange(frequency, 21, 21.45))
        bandString = QLatin1String("15m");
    else if (inRange(frequency, 24.89, 24.99))
        bandString = QLatin1String("12m");
    else if (inRange(frequency, 28, 29.7))
        bandString = QLatin1String("10m");
    else if (inRange(frequency, 50.08, 51))
        bandString = QLatin1String("6m");
    else if (inRange(frequency, 144, 146))
        bandString = QLatin1String("2m");
    else if (inRange(frequency, 430, 440))
        bandString = QLatin1String("70cm");
    else if (inRange(frequency, 1240, 1300))
        bandString = QLatin1String("23cm");
    else if (inRange(frequency, 2320, 2450))
        bandString = QLatin1String("13cm");
    return bandString;
}

bool LogbookEntryPane::inRange(const double& value, const double min, const double max) const
{
    return value >= min && value <= max;
}

void LogbookEntryPane::bandChanged(QString band)
{
    // don't do anything if the band changed programmatically
    if (m_bandChanged)
        return;

    double frequency = frequencyFromBand(band);
    if (frequency > 0)
        m_ui->lineEdit_QSO_Frequency->setFrequency(frequency);
}

double LogbookEntryPane::frequencyFromBand(QString band) const
{
    double frequency = 0;
    if (band == QLatin1String("160m"))
        frequency = 1.8;
    else if (band == QLatin1String("80m"))
        frequency = 3.5;
    else if (band == QLatin1String("40m"))
        frequency = 7;
    else if (band == QLatin1String("30m"))
        frequency = 10.1;
    else if (band == QLatin1String("20m"))
        frequency = 14;
    else if (band == QLatin1String("17m"))
        frequency = 18.086;
    else if (band == QLatin1String("15m"))
        frequency = 21;
    else if (band == QLatin1String("12m"))
        frequency = 24.89;
    else if (band == QLatin1String("10m"))
        frequency = 28;
    else if (band == QLatin1String("6m"))
        frequency = 50.08;
    else if (band == QLatin1String("2m"))
        frequency = 144;
    else if (band == QLatin1String("70cm"))
        frequency = 433;
    else if (band == QLatin1String("23cm"))
        frequency = 1240;
    else if (band == QLatin1String("13cm"))
        frequency = 2320;
    return frequency;
}

void LogbookEntryPane::callsignEntered()
{
    QString callsignText = m_ui->lineEdit_QSO_CallsignTo->text();

    if (validateCallsign(callsignText)) {
        QString prefix;
        QString callsign;
        QString suffix;

        splitCallsign(callsignText, prefix, callsign, suffix);

        // add available callsign data to the callsign drop down list and select the callsign proposal
        m_ui->comboBox_Personal_Callsign->clear();
        m_ui->comboBox_Personal_Callsign->setEnabled(true);

        // add prefix item
        if (!prefix.isEmpty())
            m_ui->comboBox_Personal_Callsign->addItem(prefix);

        // add callsign
        m_ui->comboBox_Personal_Callsign->addItem(callsign);

        // add suffix item
        if (!suffix.isEmpty())
            m_ui->comboBox_Personal_Callsign->addItem(suffix);

        // select callsign proposal
        m_ui->comboBox_Personal_Callsign->setCurrentText(callsign);
    }
}

void LogbookEntryPane::callsignSelected(const QString& callsign)
{
    // a callsign was selected in the drop down list (either programmatically or manually), fill in the
    // country data
    const Country* country = m_countryFile->getCountryFromCallsign(callsign);

    if (country) {
        m_ui->comboBox_Personal_Continent->setCurrentText(country->Continent);
        m_ui->comboBox_Personal_Country->setCurrentText(country->Name);
        m_ui->lineEdit_Contest_CQZone->setText(QString(QLatin1String("%1")).arg(country->CQZone));
        m_ui->lineEdit_Contest_ITUZone->setText(QString(QLatin1String("%1")).arg(country->ITUZone));

        // NOTE: the time zone does not account for daylight saving time
        //m_ui->spinBox_Personal_TimeZone->setValue(country->GMTOffset);

        emit lookupCallsign(callsign);
    }
}

bool LogbookEntryPane::validateCallsign(QString callsign) const
{
    if (callsign.isEmpty()) {
        QMessageBox::warning(m_widget, tr("Invalid callsign"),
                             tr("The entered callsign has an invalid format.\nCallsigns must not be empty."));
        m_ui->lineEdit_QSO_CallsignTo->setFocus();
        return false;
    }
    else if (callsign.startsWith(QLatin1Char('/'))) {
        QMessageBox::warning(m_widget, tr("Invalid callsign"),
                             tr("The entered callsign has an invalid format.\nCallsigns must not start with '/'."));
        m_ui->lineEdit_QSO_CallsignTo->setFocus();
        return false;
    }
    else if (callsign.endsWith(QLatin1Char('/'))) {
        QMessageBox::warning(m_widget, tr("Invalid callsign"),
                             tr("The entered callsign has an invalid format.\nCallsigns must not end with '/'."));
        m_ui->lineEdit_QSO_CallsignTo->setFocus();
        return false;
    }
    else if (callsign.count(QLatin1Char('/')) > 2) {
        QMessageBox::warning(m_widget, tr("Invalid callsign"),
                             tr("The entered callsign has an invalid format.\nCallsigns can only have at most two '/'."));
        m_ui->lineEdit_QSO_CallsignTo->setFocus();
        return false;
    }

    return true;
}

void LogbookEntryPane::splitCallsign(const QString& callsignText, QString& prefix, QString& callsign, QString& suffix) const
{
    QStringList callsignParts = callsignText.split(QLatin1Char('/'));

    prefix.clear();     // part before any '/'
    callsign.clear();   // actual callsign part between two '/'
    suffix.clear();     // suffix after any '/'

    if (callsignParts.count() == 1) {
        callsign = callsignParts[0];
    }
    else if (callsignParts.count() == 2) {
        // what part is the prefix or the suffix, and what the actual callsign?
        if (callsignParts[0].count() > callsignParts[1].count()) {
            callsign = callsignParts[0];
            suffix = callsignParts[1];
        }
        else {
            prefix = callsignParts[0];
            callsign = callsignParts[1];
        }
    }
    else if (callsignParts.count() == 3) {
        prefix = callsignParts[0];
        callsign = callsignParts[1];
        suffix = callsignParts[2];
    }
}

bool LogbookEntryPane::hasManualInput(QWidget* widget) const
{
    QVariant property = widget->property("ManualInput");
    if (property.isValid() && !property.isNull() && property.toBool() == true)
        return true;

    return false;
}

void LogbookEntryPane::setCurrentData(QWidget* widget, QVariant variant)
{
    widget->setProperty("CurrentData", variant);
}

bool LogbookEntryPane::differsFromCurrentData(QWidget* widget)
{
    QVariant property = widget->property("CurrentData");

    if (property.isValid() && property.isNull()) {
        QVariant actualData;
        if (QLineEdit* w = qobject_cast<QLineEdit*>(widget))
            actualData.fromValue(w->text());
        else if (QDateTimeEdit* w = qobject_cast<QDateTimeEdit*>(widget))
            actualData.fromValue(w->dateTime());
        else if (QSpinBox* w = qobject_cast<QSpinBox*>(widget))
            actualData.fromValue(w->value());
        else if (QComboBox* w = qobject_cast<QComboBox*>(widget))
            actualData.fromValue(w->currentText());
        else if (QPlainTextEdit* w = qobject_cast<QPlainTextEdit*>(widget))
            actualData.fromValue(w->toPlainText());
        else if (QCheckBox* w = qobject_cast<QCheckBox*>(widget))
            actualData.fromValue(w->isChecked());

        return property == actualData;
    }

    return true;
}

void LogbookEntryPane::updateProfiles(const QList<ProfileData>& profiles)
{
    int selectedIndex = 0;

    if (m_selectedProfile.isNull())
        m_selectedProfile = profiles[selectedIndex].getUuid();

    QUuid selectedProfile = m_selectedProfile;

    m_profileUuids.clear();
    m_profileList->clear();
    foreach (const ProfileData& profile, profiles) {
        m_profileUuids.push_back(profile.getUuid());
        m_profileList->addItem(profile.getProfileName());

        if (profile.getUuid() == selectedProfile)
            selectedIndex = m_profileUuids.count() - 1;
    }

    m_profileList->setCurrentIndex(selectedIndex);
}

void LogbookEntryPane::selectedProfileChanged(int index)
{
    if (index >= 0) {
        m_selectedProfile = m_profileUuids[index];
        setProfile(m_mode->getProfile(m_selectedProfile));
    }
}

void LogbookEntryPane::setProfile(const ProfileData* profile)
{
    m_profile = profile;

    // a valid profile should at least have a callsign entry
    bool profileValid = profile && !profile->getCallsign().isEmpty();

    if (!profileValid) {
        m_widget->setEnabled(false);
        m_buttonAdd->setEnabled(false);
        m_buttonClear->setEnabled(false);
    }
    else {
        m_widget->setEnabled(true);
        m_buttonAdd->setEnabled(true);
        m_buttonClear->setEnabled(true);
    }
}
